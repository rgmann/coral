#include "AsioTcpClient.h"

using boost::asio::ip::tcp;
using namespace liber::netapp;

// //-----------------------------------------------------------------------------
// AsioTcpClient::AsioTcpClient( boost::asio::io_service& io_service )
// :  resolver_( io_service ) {}

// //-----------------------------------------------------------------------------
// AsioTcpClient::~AsioTcpClient() {}

// //-----------------------------------------------------------------------------
// void AsioTcpClient::connect(
//    AsioTcpPacketRouterPtr  router,
//    const std::string&      host,
//    const std::string&      port )
// {
//    boost::asio::ip::tcp::resolver::query    query( host.c_str(), port );
//    boost::asio::ip::tcp::resolver::iterator iterator = resolver_.resolve( query );

//    boost::asio::async_connect(
//       router->socket(),
//       iterator,
//       boost::bind(
//         &AsioTcpPacketRouter::handleConnect,
//         router,
//         boost::asio::placeholders::error
//       )
//    );
// }

//-----------------------------------------------------------------------------
AsioTcpClientRouter::AsioTcpClientRouter( boost::asio::io_service& io_service )
   :  AsioTcpPacketRouter( io_service )
   ,  resolver_( io_service )
   ,  timer_( io_service )
{
}

//-----------------------------------------------------------------------------
AsioTcpClientRouter::~AsioTcpClientRouter() {}

//-----------------------------------------------------------------------------
void AsioTcpClientRouter::connect(
   const std::string&      host,
   const std::string&      port,
   int timeout_secs )
{
   boost::asio::ip::tcp::resolver::query    query( host, port );
   boost::asio::ip::tcp::resolver::iterator iterator = resolver_.resolve( query );

   boost::asio::async_connect(
      socket(),
      iterator,
      boost::bind(
        &AsioTcpClientRouter::handleConnect,
        shared_from_this(),
        boost::asio::placeholders::error
      )
   );

   if ( timeout_secs != kWaitForever )
   {
      timer_.expires_from_now(boost::posix_time::seconds( timeout_secs ));
      timer_.async_wait(boost::bind(
         &AsioTcpClientRouter::handleConnectTimeout,
         shared_from_this(),
         boost::asio::placeholders::error ));
   }
}

//-----------------------------------------------------------------------------
void AsioTcpClientRouter::afterConnect()
{
   timer_.cancel();
}
