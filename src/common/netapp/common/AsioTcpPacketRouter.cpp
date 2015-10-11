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



#include "Log.h"
#include "AsioTcpPacketRouter.h"

using boost::asio::ip::tcp;
using namespace liber;
using namespace liber::netapp;

//-----------------------------------------------------------------------------
AsioTcpPacketRouter::AsioTcpPacketRouter( boost::asio::io_service& io_service )
   : PacketRouter  ( this )
   , io_service_   ( io_service )
   , socket_       ( io_service )
   , connected_    ( false )
{
   memset( &read_packet_header_, 0, sizeof( read_packet_header_ ) );
}

//-----------------------------------------------------------------------------
AsioTcpPacketRouter::~AsioTcpPacketRouter()
{
}

//-----------------------------------------------------------------------------
boost::asio::ip::tcp::socket& AsioTcpPacketRouter::socket()
{
   return socket_;
}

//-----------------------------------------------------------------------------
void AsioTcpPacketRouter::start()
{
   connected_ = true;

   // Invoke on-accept callback.
   afterConnect();

   startReadHeader();
}

//-----------------------------------------------------------------------------
void AsioTcpPacketRouter::close()
{
   io_service_.post( boost::bind(
      &AsioTcpPacketRouter::doClose,
      shared_from_this()
   ));
}

//-----------------------------------------------------------------------------
bool AsioTcpPacketRouter::connected() const
{
   return connected_;
}

//-----------------------------------------------------------------------------
bool AsioTcpPacketRouter::call( PacketContainer* container_ptr )
{
   boost::mutex::scoped_lock guard( lock_ );

   bool push_success = false;

   if ( container_ptr )
   {
      writePacket( container_ptr );

      delete container_ptr;
      push_success = true;
   }

   return push_success;
}

//-----------------------------------------------------------------------------
void AsioTcpPacketRouter::startReadHeader()
{
   boost::asio::async_read(
      socket_,
      boost::asio::buffer(
         &read_packet_header_,
         sizeof( read_packet_header_ )
      ),
      boost::bind(
         &AsioTcpPacketRouter::handleReadHeader,
         shared_from_this(),
         boost::asio::placeholders::error
      )
   );
}

//-----------------------------------------------------------------------------
void AsioTcpPacketRouter::handleReadHeader(const boost::system::error_code& error)
{
   if ( !error && ( read_packet_header_.length > 0 ) )
   {
      if ( read_packet_.allocate( read_packet_header_ ) )
      {
         boost::asio::async_read( socket_,
            boost::asio::buffer(
               read_packet_.dataPtr(), 
               read_packet_header_.length
            ),
            boost::bind(
               &AsioTcpPacketRouter::handleReadBody,
               shared_from_this(),
               boost::asio::placeholders::error
            )
         );
      }

      memset( &read_packet_header_, 0, sizeof( read_packet_header_ ) );
   }
   else
   {
      doClose();
   }
}

//-----------------------------------------------------------------------------
void AsioTcpPacketRouter::handleReadBody(const boost::system::error_code& error)
{
   if ( !error )
   {
      publish(
         read_packet_.data()->type,
         read_packet_.dataPtr(),
         read_packet_.data()->length );

      read_packet_.deallocate();

      startReadHeader();
   }
   else
   {
      doClose();
   }
}

//-----------------------------------------------------------------------------
void AsioTcpPacketRouter::handleConnect(
   const boost::system::error_code& error )
{
   if ( !error )
   {
      start();
   }
}

//-----------------------------------------------------------------------------
void AsioTcpPacketRouter::handleConnectTimeout(
   const boost::system::error_code& error)
{
   if ( error.value() != boost::system::errc::success )
   {
      log::debug(
         "AsioTcpPacketRouter::handleConnectTimeout: "
         "ERROR - %s\n", error.message().c_str() );
   }

   if ( error.value() != boost::system::errc::operation_canceled )
   {
      log::debug(
         "AsioTcpPacketRouter::handleConnectTimeout: "
         "Timed out establishing connection\n" );

      doClose();
   }
}

//-----------------------------------------------------------------------------
void AsioTcpPacketRouter::writePacket( const PacketContainer* container_ptr )
{
   NetAppPacket* packet_ptr = new NetAppPacket(
      container_ptr->destination_id_,
      container_ptr->packet_ptr_->allocatedSize()
   );

   memcpy( packet_ptr->dataPtr(),
         container_ptr->packet_ptr_->basePtr(),
         container_ptr->packet_ptr_->allocatedSize() );

   // liber::log::status("write_packet:\n  header.length = %d\n  header.type = %d\n",
   //       packet_ptr->data()->length,
   //       packet_ptr->data()->type );

   io_service_.post( boost::bind(
      &AsioTcpPacketRouter::doWrite,
      shared_from_this(),
      packet_ptr
   ));
}

//-----------------------------------------------------------------------------
void AsioTcpPacketRouter::doWrite( NetAppPacket* packet_ptr )
{
   bool write_in_progress = ( write_packets_.empty() == false );

   write_packets_.push_back( packet_ptr );
   if ( !write_in_progress )
   {
      // liber::log::status("do_write:\n  header.length = %d\n  header.type = %d\n",
      //     write_packets_.front()->data()->length,
      //     write_packets_.front()->data()->type );
      boost::asio::async_write( socket_,
         boost::asio::buffer(
            write_packets_.front()->basePtr(),
            write_packets_.front()->allocatedSize()
         ),
         boost::bind(
            &AsioTcpPacketRouter::handleWrite,
            shared_from_this(),
            boost::asio::placeholders::error
         )
      );
   }
}

//-----------------------------------------------------------------------------
void AsioTcpPacketRouter::handleWrite( const boost::system::error_code& error )
{
   if (!error)
   {
      if (!write_packets_.empty() && ( write_packets_.front() != NULL ) )
      {
         delete write_packets_.front();
         write_packets_.front() = NULL;
      }

      write_packets_.pop_front();
      if ( !write_packets_.empty() )
      {
         boost::asio::async_write( socket_,
            boost::asio::buffer(
               write_packets_.front()->basePtr(),
               write_packets_.front()->allocatedSize()
            ),
            boost::bind(
               &AsioTcpPacketRouter::handleWrite,
               shared_from_this(),
               boost::asio::placeholders::error
            )
         );
      }
   }
   else
   {
      doClose();
   }
}

//-----------------------------------------------------------------------------
void AsioTcpPacketRouter::doClose()
{
   beforeClose();

   connected_ = false;

   socket_.close();
}
