// 
// Copyright (c) 2015, Robert Glissmann
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// 
// * Redistributions of source code must retain the above copyright notice, this
// list of conditions and the following disclaimer.
// 
// * Redistributions in binary form must reproduce the above copyright notice,
// this list of conditions and the following disclaimer in the documentation
// and/or other materials provided with the distribution.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// 

// %% license-end-token %%
// 
// Author: Robert.Glissmann@gmail.com (Robert Glissmann)
// 
// 



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

namespace coral {
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
} // namespace coral


#endif // ASIO_TCP_PACKET_ROUTER_H