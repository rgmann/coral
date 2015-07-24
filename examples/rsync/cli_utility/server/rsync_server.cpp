#include <algorithm>
#include <cstdlib>
#include <deque>
#include <iostream>
#include <list>
#include <set>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>
#include "Log.h"
#include "IThread.h"
#include "NetAppPacket.h"
#include "PacketQueue.h"
#include "RsyncNode.h"

#define  RSYNC_SUB_ID  ( 1 )

using boost::asio::ip::tcp;
using namespace liber::netapp;
using namespace liber::rsync;

//----------------------------------------------------------------------

typedef std::deque<NetAppPacket*> packet_queue;

class AsioRsyncClient {
public:

  virtual ~AsioRsyncClient(){};

};

typedef boost::shared_ptr<AsioRsyncClient> AsioRsyncClientPtr;

//----------------------------------------------------------------------
class AsioRsyncClientManager {
public:

  void join( AsioRsyncClientPtr client )
  {
    liber::log::status("JOINED\n");
    clients_.insert( client );
  }

  void leave( AsioRsyncClientPtr client )
  {
    liber::log::status("LEAVING\n");
    clients_.erase( client );
  }

private:

  std::set<AsioRsyncClientPtr> clients_;
};

//----------------------------------------------------------------------

class AsioRsyncServerSession :
public AsioRsyncClient,
public PacketRouter,
public liber::concurrency::IThread,
public boost::enable_shared_from_this<AsioRsyncServerSession> {
public:

  AsioRsyncServerSession( boost::asio::io_service& io_service, AsioRsyncClientManager& client_manager )
  : PacketRouter( &packet_receiver_ )
  , IThread( "asio_rsync_server_sessions" )
  , io_service_( io_service )
  , socket_(io_service)
  , client_manager_(client_manager)
  , rsync_node_( boost::filesystem::current_path() / "rsync_server_root" )
  {
    addSubscriber( RSYNC_SUB_ID, &rsync_node_.subscriber() );
  }

  ~AsioRsyncServerSession()
  {
    cancel(true);
    removeSubscriber( RSYNC_SUB_ID );
  }

  tcp::socket& socket()
  {
    return socket_;
  }

  void start()
  {
    launch();

    client_manager_.join( shared_from_this() );
    boost::asio::async_read(
      socket_,
      boost::asio::buffer(
        &read_packet_header_,
        sizeof( read_packet_header_ )
      ),
      boost::bind(
        &AsioRsyncServerSession::handle_read_header,
        shared_from_this(),
        boost::asio::placeholders::error
      )
    );
  }

  // void deliver(const chat_message& msg)
  // {
  //   bool write_in_progress = !write_packets_.empty();
  //   write_packets_.push_back(msg);
  //   if (!write_in_progress)
  //   {
  //     boost::asio::async_write(socket_,
  //         boost::asio::buffer(write_packets_.front().data(),
  //           write_packets_.front().length()),
  //         boost::bind(&chat_session::handle_write, shared_from_this(),
  //           boost::asio::placeholders::error));
  //   }
  // }
  void write_packet( const PacketContainer* container_ptr )
  {
    NetAppPacket* packet_ptr = new NetAppPacket(
      container_ptr->mDestinationID,
      container_ptr->mpPacket->allocatedSize()
    );

    memcpy( packet_ptr->dataPtr(),
           container_ptr->mpPacket->basePtr(),
           container_ptr->mpPacket->allocatedSize() );

    io_service_.post( boost::bind( &AsioRsyncServerSession::do_write, this, packet_ptr ) );
  }

  void run( const bool& shutdown )
  {
    while ( !shutdown )
    {
      PacketContainer* container_ptr = NULL;
      if ( ( container_ptr = packet_receiver_.pop() ) != NULL) {

        write_packet( container_ptr );

        delete container_ptr;
      }
    }
  }

  void handle_read_header(const boost::system::error_code& error)
  {
    if ( !error && ( read_packet_header_.length > 0 ) )
    {
      // liber::log::status("handle_read_header:\n  header.length = %d\n  header.type = %\n",
      //   read_packet_header_.length,
      //   read_packet_header_.type);
      if ( read_packet_.allocate( read_packet_header_ ) )
      {
        boost::asio::async_read(
          socket_,
          boost::asio::buffer(
            read_packet_.dataPtr(),
            read_packet_header_.length
          ),
          boost::bind(
            &AsioRsyncServerSession::handle_read_body,
            shared_from_this(),
            boost::asio::placeholders::error
          )
        );
      }

      memset( &read_packet_header_, 0, sizeof( read_packet_header_ ) );
    }
    else
    {
      client_manager_.leave(shared_from_this());
    }
  }

  void handle_read_body(const boost::system::error_code& error)
  {
    if (!error)
    {
      // liber::log::status("handle_read_body:\n  packet.type = %d\n  packet.length = %d\n",
      //   read_packet_.data()->type, read_packet_.data()->length);
      PacketSubscriber* subscriber_ptr = getSubscriber( read_packet_.data()->type );

      if ( subscriber_ptr )
      {
        subscriber_ptr->put(
          (char*)read_packet_.dataPtr(),
          read_packet_.data()->length
        );
      }

      read_packet_.destroy();

      boost::asio::async_read(
        socket_,
        boost::asio::buffer(
          &read_packet_header_,
          sizeof( read_packet_header_ )
        ),
        boost::bind(
          &AsioRsyncServerSession::handle_read_header,
          shared_from_this(),
          boost::asio::placeholders::error
        )
      );
    }
    else
    {
      client_manager_.leave(shared_from_this());
    }
  }

  void do_write( NetAppPacket* packet_ptr )
  {
    bool write_in_progress = !write_packets_.empty();
    write_packets_.push_back( packet_ptr );
    if ( !write_in_progress )
    {
      boost::asio::async_write(
        socket_,
        boost::asio::buffer(
          write_packets_.front()->basePtr(),
          write_packets_.front()->allocatedSize()
        ),
        boost::bind(
          &AsioRsyncServerSession::handle_write,
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
            &AsioRsyncServerSession::handle_write,
            shared_from_this(),
            boost::asio::placeholders::error
          )
        );
      }
    }
    else
    {
      client_manager_.leave(shared_from_this());
    }
  }

private:
  boost::asio::io_service& io_service_;
  tcp::socket socket_;
  AsioRsyncClientManager& client_manager_;
  NetAppPacket::Data      read_packet_header_;
  NetAppPacket            read_packet_;
  packet_queue            write_packets_;
  PacketQueue             packet_receiver_;

  RsyncNode               rsync_node_;
};

typedef boost::shared_ptr<AsioRsyncServerSession> AsioRsyncServerSessionPtr;

//----------------------------------------------------------------------

class AsioRsyncServer {
public:
  AsioRsyncServer( boost::asio::io_service& io_service, const tcp::endpoint& endpoint )
  : io_service_( io_service )
  , acceptor_  ( io_service, endpoint )
  {
    start_accept();
  }

  void start_accept()
  {
    AsioRsyncServerSessionPtr new_session(new AsioRsyncServerSession(io_service_, client_manager_));
    acceptor_.async_accept(
      new_session->socket(),
      boost::bind(
        &AsioRsyncServer::handle_accept,
        this,
        new_session,
        boost::asio::placeholders::error
      )
    );
  }

  void handle_accept( AsioRsyncServerSessionPtr session, const boost::system::error_code& error )
  {
    if (!error)
    {
      session->start();
    }

    start_accept();
  }

private:
  boost::asio::io_service& io_service_;
  tcp::acceptor acceptor_;
  // chat_room room_;
  AsioRsyncClientManager client_manager_;
};

typedef boost::shared_ptr<AsioRsyncServer> AsioRsyncServerPtr;
typedef std::list<AsioRsyncServerPtr> AsioRsyncServerList;

//----------------------------------------------------------------------

int main(int argc, char* argv[])
{
  liber::log::level( liber::log::Verbose );
  try
  {
    if (argc < 2)
    {
      std::cerr << "Usage: rsync_server <port> [<port> ...]\n";
      return 1;
    }

    boost::asio::io_service io_service;

    AsioRsyncServerList servers;
    for (int i = 1; i < argc; ++i)
    {
      using namespace std; // For atoi.
      tcp::endpoint endpoint(tcp::v4(), atoi(argv[i]));
      AsioRsyncServerPtr server(new AsioRsyncServer( io_service, endpoint));
      servers.push_back(server);
    }

    io_service.run();
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}
