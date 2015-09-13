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
#include "NetAppPacket.h"
#include "PacketRouter.h"
#include "PacketReceiverHook.h"

namespace liber {
namespace netapp {

class AsioTcpPacketRouter :
public PacketRouter,
public PacketReceiverHook,
public boost::enable_shared_from_this<AsioTcpPacketRouter>  {
public:

   AsioTcpPacketRouter( boost::asio::io_service&  io_service );

   virtual ~AsioTcpPacketRouter();

   boost::asio::ip::tcp::socket& socket();

   void start();

   void close();

   bool connected() const;

protected:

   virtual void afterConnect() {}

   virtual void beforeClose() {}

   virtual bool call( PacketContainer* container_ptr );


protected:

   void handleConnect( const boost::system::error_code& error );

   void handleConnectTimeout(const boost::system::error_code&);

   void writePacket( const PacketContainer* container_ptr );

   void startReadHeader();

   void handleReadHeader( const boost::system::error_code& error );

   void handleReadBody( const boost::system::error_code& error );

   void doWrite( NetAppPacket* packet_ptr );

   void handleWrite(const boost::system::error_code& error);

   void doClose();


private:

   boost::asio::io_service& io_service_;
   boost::asio::ip::tcp::socket socket_;
   NetAppPacket::Data read_packet_header_;
   NetAppPacket                read_packet_;
   std::deque<NetAppPacket*>   write_packets_;
   boost::mutex                lock_;
   bool connected_;
};

typedef boost::shared_ptr<AsioTcpPacketRouter> AsioTcpPacketRouterPtr;

} // namespace netapp
} // namespace liber


#endif // ASIO_TCP_PACKET_ROUTER_H