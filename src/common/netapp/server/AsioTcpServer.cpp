#include "AsioTcpServer.h"

using namespace liber::netapp;

//-----------------------------------------------------------------------------
AsioTcpServer::AsioTcpServer(
   boost::asio::io_service&   io_service,
   const boost::asio::ip::tcp::endpoint&       endpoint )
:  io_service_( io_service )
,  acceptor_  ( io_service, endpoint )
{
}

//-----------------------------------------------------------------------------
AsioTcpServer::~AsioTcpServer()
{
}

//-----------------------------------------------------------------------------
void AsioTcpServer::startAccept()
{
   AsioTcpPacketRouterPtr connection = createRouter( io_service_ ) ;

   acceptor_.async_accept(
      connection->socket(),
      boost::bind(
        &AsioTcpServer::handleAccept,
        this,
        connection,
        boost::asio::placeholders::error
      )
   );
}

//-----------------------------------------------------------------------------
void AsioTcpServer::handleAccept(
   AsioTcpPacketRouterPtr           connection,
   const boost::system::error_code& error )
{
   if ( !error )
   {
      connection->start();
   }

   startAccept();
}
