#include <cstdlib>
#include <deque>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/locks.hpp>
#include "IThread.h"
#include "Log.h"
#include "NetAppPacket.h"
#include "PacketQueue.h"
#include "RsyncNode.h"
#include "InteractiveCommandRouter.h"


using boost::asio::ip::tcp;
using namespace liber::netapp;
using namespace liber::rsync;
using namespace liber::cli;

typedef std::deque<NetAppPacket*> packet_queue;

#define  RSYNC_SUB_ID  ( 1 )
//public liber::concurrency::IThread,
class AsioTcpPacketRouter : 
public liber::netapp::PacketRouter,
public liber::netapp::PacketReceiver {
public:

  AsioTcpPacketRouter(
    boost::asio::io_service& io_service,
    tcp::resolver::iterator endpoint_iterator)
  //: IThread("asio_tcp_packet_router")
  : PacketRouter  ( this )//( &mReceiver )
  , io_service_   ( io_service )
  , socket_       ( io_service )
  {
    memset( &read_packet_header_, 0, sizeof( read_packet_header_ ) );

    boost::asio::async_connect(
      socket_,
      endpoint_iterator,
      boost::bind(
        &AsioTcpPacketRouter::handle_connect,
        this,
        boost::asio::placeholders::error
      )
    );
  }

  void close()
  {
    io_service_.post( boost::bind( &AsioTcpPacketRouter::do_close, this ) );
  }


protected:

  bool push( PacketContainer* container_ptr )
  {
    boost::mutex::scoped_lock guard( lock_ );

    bool push_success = false;
    if ( container_ptr )
    {
      write_packet( container_ptr );
      delete container_ptr;
      push_success = true;
    }

    return push_success;
  }

  void handle_connect(const boost::system::error_code& error)
  {
    if ( !error )
    {
      boost::asio::async_read(
        socket_,
        boost::asio::buffer(
          &read_packet_header_,
          sizeof( read_packet_header_ )
        ),
        boost::bind(
          &AsioTcpPacketRouter::handle_read_header,
          this,
          boost::asio::placeholders::error
        )
      );
    }
  }

  void handle_read_header(const boost::system::error_code& error)
  {
    if ( !error && ( read_packet_header_.length > 0 ) )
    {
      // liber::log::status("handle_read_header: read_packet_header_.length = %d\n", read_packet_header_.length);
      if ( read_packet_.allocate( read_packet_header_ ) )
      {
        boost::asio::async_read(
          socket_,
          boost::asio::buffer(
            read_packet_.dataPtr(), 
            read_packet_header_.length
          ),
          boost::bind(
            &AsioTcpPacketRouter::handle_read_body,
            this,
            boost::asio::placeholders::error
          )
        );
      }

      memset( &read_packet_header_, 0, sizeof( read_packet_header_ ) );
    }
    else
    {
      do_close();
    }
  }

  void handle_read_body(const boost::system::error_code& error)
  {
    if (!error)
    {
      PacketSubscriber* subscriber_ptr = getSubscriber( read_packet_.data()->type );
      if ( subscriber_ptr )
      {
        subscriber_ptr->put(
          (char*)read_packet_.dataPtr(),
          read_packet_.data()->length
        );
      }

      read_packet_.destroy();

      //
      // Start reading the next packet.
      //
      boost::asio::async_read(
        socket_,
        boost::asio::buffer(
          &read_packet_header_,
          sizeof( read_packet_header_ )
        ),
        boost::bind(
          &AsioTcpPacketRouter::handle_read_header,
          this,
          boost::asio::placeholders::error
        )
      );
    }
    else
    {
      do_close();
    }
  }


  void write_packet( const PacketContainer* container_ptr )
  {
    NetAppPacket* packet_ptr = new NetAppPacket(
      container_ptr->mDestinationID,
      container_ptr->mpPacket->allocatedSize()
    );

    memcpy( packet_ptr->dataPtr(),
           container_ptr->mpPacket->basePtr(),
           container_ptr->mpPacket->allocatedSize() );

    // liber::log::status("write_packet:\n  header.length = %d\n  header.type = %d\n",
    //       packet_ptr->data()->length,
    //       packet_ptr->data()->type );

    io_service_.post( boost::bind( &AsioTcpPacketRouter::do_write, this, packet_ptr ) );
  }

  // void run( const bool& shutdown )
  // {
  //   while ( !shutdown ) {
  //     PacketContainer* container_ptr = NULL;
  //     if ( ( container_ptr = mReceiver.pop() ) != NULL) {

  //       write_packet( container_ptr );

  //       delete container_ptr;
  //     }
  //   }
  // }

  void do_write( NetAppPacket* packet_ptr )
  {
    bool write_in_progress = !write_packets_.empty();
    write_packets_.push_back( packet_ptr );
    if ( !write_in_progress )
    {
      // liber::log::status("do_write:\n  header.length = %d\n  header.type = %d\n",
      //     write_packets_.front()->data()->length,
      //     write_packets_.front()->data()->type );
      boost::asio::async_write(
        socket_,
        boost::asio::buffer(
          write_packets_.front()->basePtr(),
          write_packets_.front()->allocatedSize()
        ),
        boost::bind(
          &AsioTcpPacketRouter::handle_write,
          this,
          boost::asio::placeholders::error
        )
      );
    }
  }

  void handle_write(const boost::system::error_code& error)
  {
    if (!error)
    {
      if (!write_packets_.empty() && ( write_packets_.front() != NULL ) ) {
        delete write_packets_.front();
        write_packets_.front() = NULL;
      }

      write_packets_.pop_front();
      if (!write_packets_.empty())
      {
        // liber::log::status("handle_write:\n  header.length = %d\n  header.type = %d\n",
        //   write_packets_.front()->data()->length,
        //   write_packets_.front()->data()->type );
        boost::asio::async_write(
          socket_,
          boost::asio::buffer(
            write_packets_.front()->basePtr(),
            write_packets_.front()->allocatedSize()
          ),
          boost::bind(
            &AsioTcpPacketRouter::handle_write,
            this,
            boost::asio::placeholders::error
          )
        );
      }
    }
    else
    {
      do_close();
    }
  }

  void do_close()
  {
    socket_.close();
  }

private:

  boost::asio::io_service& io_service_;
  tcp::socket socket_;
  NetAppPacket::Data read_packet_header_;
  NetAppPacket read_packet_;
  packet_queue write_packets_;
  PacketQueue mReceiver;
  boost::mutex lock_;
};

class CompletionCallback : public RsyncJobCallback {
public:

  CompletionCallback(const char* pName, CountingSem& semaphore ) : semaphore_(semaphore), mpName(pName) {};

  void call(const JobDescriptor& job, const JobReport& report)
  {
    liber::log::status("%s: Completed %s\n", mpName, job.getDestination().path.string().c_str());
    liber::log::flush();

    report.print(std::cout);
    std::cout << "\n\n";

    semaphore_.give();
  }

  CountingSem& semaphore_;
  const char* mpName;
};

class ClientAttributes {
public:
  ClientAttributes( RsyncNode* node_ptr )
  : node_ptr_ ( node_ptr )
  , semaphore_( 0 )
  , callback_ ( "job_callback", semaphore_ )
  {
    node_ptr_->setCallback( &callback_ );
  }
  ~ClientAttributes() {
  }

  bool is_connected() const
  {
    return true;
  }

  RsyncNode*  node_ptr_;
  CountingSem semaphore_;
  CompletionCallback callback_;
};

class PushCommand : public InteractiveCommand {
public:

  PushCommand( ClientAttributes& attributes )
  : InteractiveCommand( "push", "Push a file to the server" )
  , attributes_( attributes ) {};

  void process(const liber::cli::ArgumentList& args) {
    if ( attributes_.is_connected() ) {
      if ( attributes_.node_ptr_->push( args[ 0 ] ) == kRsyncSuccess ) {
        attributes_.semaphore_.take();
        liber::log::status("Done\n");
      } else {
        liber::log::status("Failed\n");
      }
    } else {
      liber::log::status("Push failed: client is not connected\n");
    }
  }

private:

  ClientAttributes& attributes_;
};

class PullCommand : public InteractiveCommand {
public:

  PullCommand( ClientAttributes& attributes )
  : InteractiveCommand( "pull", "Push a file to the server" )
  , attributes_( attributes ) {};

  void process(const liber::cli::ArgumentList& args) {
    if ( attributes_.is_connected() ) {
      if ( attributes_.node_ptr_->pull( args[ 0 ] ) == kRsyncSuccess ) {
        attributes_.semaphore_.take();
        liber::log::status("Done\n");
      } else {
        liber::log::status("Failed\n");
      }
    } else {
      liber::log::status("Pull failed: client is not connected\n");
    }
  }

private:

  ClientAttributes& attributes_;
};


int main(int argc, char* argv[])
{
  liber::log::level( liber::log::Verbose );
  try
  {
    if (argc != 3)
    {
      std::cerr << "Usage: rsync_client <host> <port> <push|pull> <path>\n";
      return 1;
    }

    boost::asio::io_service io_service;

    tcp::resolver resolver(io_service);
    tcp::resolver::query query(argv[1], argv[2]);
    tcp::resolver::iterator iterator = resolver.resolve(query);

    AsioTcpPacketRouter router( io_service, iterator );

    boost::thread t(boost::bind(&boost::asio::io_service::run, &io_service));

    RsyncNode* rsync_node_ptr = new RsyncNode( boost::filesystem::current_path() / "rsync_client_root" );
    ClientAttributes attributes( rsync_node_ptr );

    router.addSubscriber(RSYNC_SUB_ID, &rsync_node_ptr->subscriber());
    // router.launch();

    PushCommand push_command( attributes );
    PullCommand pull_command( attributes );

    InteractiveCommandRouter command_router;
    command_router.add( &push_command );
    command_router.add( &pull_command );

    command_router.run();


    router.removeSubscriber( RSYNC_SUB_ID );

    router.close();
    // router.cancel(true);
    t.join();

    rsync_node_ptr->unsetCallback();
    delete rsync_node_ptr;
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}
