#include <cstdlib>
#include <deque>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/locks.hpp>
#include "Log.h"
#include "ArgParser.h"
#include "AsioTcpClient.h"
#include "RsyncNode.h"
#include "InteractiveCommandRouter.h"


using boost::asio::ip::tcp;
using namespace coral::netapp;
using namespace coral::rsync;
using namespace coral::cli;
using namespace coral::thread;


#define  RSYNC_SUB_ID  ( 1 )


class CompletionCallback : public RsyncJobCallback {
public:

   CompletionCallback(const char* name, CountingSem& semaphore )
      : semaphore_( semaphore ), name_( name ) {};

   void call(const JobDescriptor& job, const JobReport& report)
   {
      coral::log::status("%s: Completed %s\n", name_, job.getDestination().path().c_str());

      semaphore_.give();
   }

   CountingSem& semaphore_;
   const char* name_;
};

class ClientAttributes {
public:

   ClientAttributes( RsyncNode* node_ptr, AsioTcpClientRouterPtr client )
      : node_ptr_ ( node_ptr )
      , semaphore_( 0 )
      , callback_ ( "job_callback", semaphore_ )
      , client_( client )
   {
      node_ptr_->setJobCompletionCallback( &callback_ );
   }
   ~ClientAttributes() {}

   bool is_connected() const
   {
      return client_->connected();
   }

   RsyncNode*  node_ptr_;
   CountingSem semaphore_;
   CompletionCallback callback_;
   AsioTcpClientRouterPtr  client_;
};

class PushCommand : public InteractiveCommand {
public:

  PushCommand( ClientAttributes& attributes )
     : InteractiveCommand( "push", "Push a file to the server" )
     , attributes_( attributes ) {};

   void process(const coral::cli::ArgumentList& args)
   {
      if ( attributes_.is_connected() ) {
         if ( attributes_.node_ptr_->push( args[ 0 ] ) == kRsyncSuccess ) {
            attributes_.semaphore_.take();
            coral::log::status("Done\n");
         } else {
            coral::log::status("Failed\n");
         }
      } else {
         coral::log::status("Push failed: client is not connected\n");
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

   void process(const coral::cli::ArgumentList& args) {
      if ( attributes_.is_connected() ) {
         if ( attributes_.node_ptr_->pull( args[ 0 ] ) == kRsyncSuccess ) {
            attributes_.semaphore_.take();
            coral::log::status("Done\n");
         } else {
            coral::log::status("Failed\n");
         }
      } else {
         coral::log::status("Pull failed: client is not connected\n");
      }
   }

private:

   ClientAttributes& attributes_;
};

class ConnectionStatusCommand : public InteractiveCommand {
public:

   ConnectionStatusCommand( ClientAttributes& attributes )
      : InteractiveCommand( "status", "Check connection status" )
      , attributes_( attributes ) {}

   void process( const coral::cli::ArgumentList& args ) {
      if ( attributes_.is_connected() ) {
         coral::log::status("Connected to server.\n");
      } else {
         coral::log::status("Not connected to server.\n");
      }
   }

private:

   ClientAttributes& attributes_;
};


int main(int argc, char* argv[])
{
   coral::log::level( coral::log::Verbose );

   ArgParser args;
   args.addArg("name: Port, primary: p, alt: port, type: opt, \
               vtype: string, required, desc: Set port number");
   args.addArg("name: Host, primary: h, alt: host, type: opt, \
               vtype: string, required, desc: Host name/address");

   if ( args.parse( (const char**)argv, argc ) )
   {
      std::string host_name;
      std::string host_port;
      
      args.getArgVal( Argument::ArgName, "Host", host_name );
      args.getArgVal( Argument::ArgName, "Port", host_port );

      try
      {
         boost::asio::io_service io_service;
         // AsioTcpPacketRouterPtr router( new AsioTcpPacketRouter( io_service ) );
         // AsioTcpClient client( io_service );
         AsioTcpClientRouterPtr client_router( new AsioTcpClientRouter( io_service ) );

         // client.connect( router, host_name, host_port );
         client_router->connect( host_name, host_port, AsioTcpClientRouter::kWaitForever );

         boost::thread t(boost::bind( &boost::asio::io_service::run, &io_service));

         WorkerGroup work_group;
         RsyncNode* rsync_node_ptr = new RsyncNode( boost::filesystem::current_path() / "rsync_client_root", work_group );
         ClientAttributes attributes( rsync_node_ptr, client_router );

         client_router->subscribe( RSYNC_SUB_ID, &rsync_node_ptr->subscriber() );

         PushCommand push_command( attributes );
         PullCommand pull_command( attributes );
         ConnectionStatusCommand status_command( attributes );

         InteractiveCommandRouter command_router;
         command_router.add( &push_command );
         command_router.add( &pull_command );
         command_router.add( &status_command );

         command_router.run();

         client_router->unsubscribe( RSYNC_SUB_ID, &rsync_node_ptr->subscriber() );

         client_router->close();

         t.interrupt();
         t.join();

         rsync_node_ptr->unsetJobCompletionCallback();
         delete rsync_node_ptr;
      }
      catch (std::exception& e)
      {
         coral::log::error( "Exception: %s\n", e.what() );
      }
   }
   else
   {
      if ( args.helpRequested() )
      {
         coral::log::raw( args.printHelp().c_str() );
      }
      else
      {
         coral::log::raw( args.printArgErrors(true).c_str() );
      }
      
   }

   coral::log::flush();

   return 0;
}
