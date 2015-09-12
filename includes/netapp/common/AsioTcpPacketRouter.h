#ifndef  ASIO_TCP_PACKET_ROUTER_H
#define  ASIO_TCP_PACKET_ROUTER_H

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
// #include "IThread.h"
#include "NetAppPacket.h"
#include "PacketRouter.h"
#include "PacketReceiverHook.h"

namespace liber {
namespace netapp {


class AsioTcpPacketRouter :
public liber::netapp::PacketRouter,
public liber::netapp::PacketReceiverHook,
public boost::enable_shared_from_this<AsioTcpPacketRouter>  {
public:

   AsioTcpPacketRouter( boost::asio::io_service&  io_service );

   virtual ~AsioTcpPacketRouter();

   boost::asio::ip::tcp::socket& socket();

   void start();

   void close();


protected:

   virtual void afterAccept() {}

   virtual void beforeClose() {}

   virtual bool call( PacketContainer* container_ptr );


private:

   void handleConnect( const boost::system::error_code& error )
   {
      if ( !error )
      {
         start();
      }
   }

   void writePacket( const PacketContainer* container_ptr );

   void startReadHeader();

   void handleReadHeader( const boost::system::error_code& error );

   void handleReadBody( const boost::system::error_code& error );

   void doWrite( NetAppPacket* packet_ptr );

   void handleWrite(const boost::system::error_code& error);

   void doClose();

   friend class AsioTcpClient;

private:

   boost::asio::io_service& io_service_;
   boost::asio::ip::tcp::socket socket_;
   NetAppPacket::Data read_packet_header_;
   NetAppPacket                read_packet_;
   std::deque<NetAppPacket*>   write_packets_;
   boost::mutex                lock_;
};

typedef boost::shared_ptr<AsioTcpPacketRouter> AsioTcpPacketRouterPtr;


class AsioTcpClient {
public:

   AsioTcpClient( boost::asio::io_service& io_service )
      :  resolver_( io_service ) {}

   ~AsioTcpClient() {}

   void connect( AsioTcpPacketRouterPtr router, const std::string& host, const std::string port )
   {
      boost::asio::ip::tcp::resolver::query query( host.c_str(), port );
      boost::asio::ip::tcp::resolver::iterator iterator = resolver_.resolve(query);

      boost::asio::async_connect(
         router->socket(),
         iterator,
         boost::bind(
           &AsioTcpPacketRouter::handleConnect,
           router,
           boost::asio::placeholders::error
         )
      );
   }

private:

   boost::asio::ip::tcp::resolver resolver_;
    
};


class AsioTcpServer {
public:

   AsioTcpServer(
      boost::asio::io_service&              io_service,
      const boost::asio::ip::tcp::endpoint& endpoint );
   virtual ~AsioTcpServer();

   void startAccept();


protected:

   virtual AsioTcpPacketRouterPtr createRouter( boost::asio::io_service& io_service ) = 0;

private:

   void handleAccept( AsioTcpPacketRouterPtr session, const boost::system::error_code& error );

private:

   boost::asio::io_service& io_service_;

   boost::asio::ip::tcp::acceptor acceptor_;

};

typedef boost::shared_ptr<AsioTcpServer> AsioTcpServerPtr;


}
}


#endif // ASIO_TCP_PACKET_ROUTER_H