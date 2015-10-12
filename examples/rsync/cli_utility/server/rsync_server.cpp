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
#include "ArgParser.h"
#include "AsioTcpServer.h"
#include "RsyncNode.h"

#define  RSYNC_SUB_ID  ( 1 )

using boost::asio::ip::tcp;
using namespace liber::netapp;
using namespace liber::rsync;
using namespace liber::cli;

//----------------------------------------------------------------------

class AsioRsyncClient  {
public:

  virtual ~AsioRsyncClient(){};

  virtual void push_file( const boost::filesystem::path& path ) = 0;

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

   void push_file( const boost::filesystem::path& path )
   {
      recent_pushes_.push_back( path );

      std::for_each( clients_.begin(), clients_.end(),
         boost::bind( &AsioRsyncClient::push_file, _1, boost::ref( path )));
   }

   WorkerGroup& worker_group()
   {
      return worker_group_;
   }


private:

   std::set<AsioRsyncClientPtr> clients_;
   std::deque<boost::filesystem::path> recent_pushes_;

   WorkerGroup worker_group_;
};

class JobCallback : public RsyncJobCallback, public AsioRsyncClient {
public:

   JobCallback( AsioRsyncClientManager& client_manager )
      : client_manager_(client_manager)
      , rsync_node_( boost::filesystem::current_path() / "rsync_server_root", client_manager.worker_group() )
   {
      rsync_node_.setJobCompletionCallback( this );
   }

   ~JobCallback()
   {
      rsync_node_.unsetJobCompletionCallback();
   }

   void call(const JobDescriptor& job, const JobReport& report)
   {
      if ( job.getDestination().remote() == true )
      {
         client_manager_.push_file( job.getDestinationPath() );
      }
   }

   void push_file( const boost::filesystem::path& path )
   {
      if ( rsync_node_.push( path ) != kRsyncSuccess )
      {
         liber::log::status( "Failed to push file: %s\n", path.string().c_str() );
      }
   }

   AsioRsyncClientManager& client_manager_;
   RsyncNode               rsync_node_;
};

typedef boost::shared_ptr<JobCallback> JobCallbackPtr;

//----------------------------------------------------------------------
class AsioRsyncServerSession :
public AsioTcpPacketRouter  {
public:

   AsioRsyncServerSession( boost::asio::io_service& io_service, AsioRsyncClientManager& client_manager )
      : AsioTcpPacketRouter( io_service )
      , client_manager_( client_manager )
      , callback_( new JobCallback( client_manager ) )
   {
      subscribe( RSYNC_SUB_ID, &callback_->rsync_node_.subscriber() );
   }

   ~AsioRsyncServerSession()
   {
      unsubscribe( RSYNC_SUB_ID, &callback_->rsync_node_.subscriber() );
   }


protected:

   void afterConnect()
   {
      client_manager_.join( callback_ );
   }

   void beforeClose()
   {
      client_manager_.leave( callback_ );
   }

private:

   AsioRsyncClientManager& client_manager_;
   JobCallbackPtr             callback_;
};

typedef boost::shared_ptr<AsioRsyncServerSession> AsioRsyncServerSessionPtr;

//----------------------------------------------------------------------

class AsioRsyncServer : public AsioTcpServer {
public:

   AsioRsyncServer( boost::asio::io_service& io_service, const tcp::endpoint& endpoint )
   : AsioTcpServer( io_service, endpoint )
   {
   }

protected:

   AsioTcpPacketRouterPtr createRouter( boost::asio::io_service& io_service )
   {
      return AsioTcpPacketRouterPtr( new AsioRsyncServerSession( io_service, client_manager_ ) );
   }

private:

   AsioRsyncClientManager client_manager_;
};

typedef boost::shared_ptr<AsioRsyncServer>   AsioRsyncServerPtr;
typedef std::list<AsioRsyncServerPtr>        AsioRsyncServerList;


//----------------------------------------------------------------------
int main( int argc, char* argv[] )
{
   liber::log::level( liber::log::Verbose );

   ArgParser args;
   args.addArg("name: Port, primary: p, alt: port, type: opt, \
               vtype: int, required, desc: Set port number");
   
   if ( args.parse( (const char**)argv, argc ) )
   {
      try
      {
         int host_port = 0;
         args.getArgVal( Argument::ArgName, "Port", host_port );

         boost::asio::io_service io_service;
         tcp::endpoint endpoint( tcp::v4(), host_port );

         liber::log::status( "Starting server on port %d\n", host_port );
         AsioRsyncServerPtr server( new AsioRsyncServer( io_service, endpoint ) );
         server->startAccept();

         io_service.run();
      }
      catch (std::exception& e)
      {
         liber::log::error( "Exception: %s\n", e.what() );
      }
   }
   else
   {
      args.printArgErrors(true);
      return 1;
   }

   return 0;
}
