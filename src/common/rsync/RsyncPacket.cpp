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



#include <stdio.h>
#include "Log.h"
#include "RsyncPacket.h"
#include "ByteOrder.h"

using namespace liber;
using namespace liber::net;
using namespace liber::netapp;
using namespace liber::rsync;



//-----------------------------------------------------------------------------
RsyncTransportPacket::RsyncTransportPacket( int type, const GenericPacket* packet_ptr )
   : GenericPacket( sizeof( RsyncTransportPacket::Data ), packet_ptr->allocatedSize() )
{
   if ( isAllocated() )
   {
      data()->type   = type;
      data()->length = packet_ptr->allocatedSize();

      if ( packet_ptr->allocatedSize() > 0 )
      {
         memcpy(dataPtr(), packet_ptr->basePtr(), packet_ptr->allocatedSize());
      }
   }
}

//-----------------------------------------------------------------------------
void RsyncTransportPacket::swap( void* data_ptr, ui32 length )
{
   RsyncTransportPacket::Data* header_ptr = NULL;

   if ( data_ptr && (length >= sizeof(RsyncTransportPacket::Data)) )
   {
      header_ptr = reinterpret_cast<RsyncTransportPacket::Data*>(data_ptr);

      header_ptr->type   = ByteOrder::NetSwap(header_ptr->type);
      header_ptr->length = ByteOrder::NetSwap(header_ptr->length);
   }
}

//-----------------------------------------------------------------------------
RsyncTransportPacket::Data* const RsyncTransportPacket::data() const
{
   Data* header_ptr = NULL;

   if ( isAllocated() )
   {
      header_ptr = reinterpret_cast<Data*>( basePtr() );
   }

   return header_ptr;
}


//-----------------------------------------------------------------------------
RsyncPacket::RsyncPacket()
   : GenericPacket( sizeof( RsyncPacket::Data ), 0 )
{
}

//-----------------------------------------------------------------------------
RsyncPacket::RsyncPacket(int type, ui32 length, const void* pData)
   : GenericPacket(sizeof(RsyncPacket::Data), length)
{
   if (isAllocated())
   {
      data()->type = type;
      data()->length = length;

      if (pData)
      {
         memcpy(dataPtr(), pData, length);
      }
   }
}

//-----------------------------------------------------------------------------
RsyncPacket::RsyncPacket(int type, const std::string& rData)
   : GenericPacket( sizeof(RsyncPacket::Data), rData.size() )
{
   if ( isAllocated() )
   {
      data()->type = type;
      data()->length = rData.size();

      if ( rData.size() > 0 )
      {
         memcpy( dataPtr(), rData.data(), rData.size() );
      }
   }
}

//-----------------------------------------------------------------------------
RsyncPacket::Data* const RsyncPacket::data() const
{
   Data* header_ptr = NULL;

   if ( isAllocated() )
   {
      header_ptr = reinterpret_cast<Data*>( basePtr() );
   }

   return header_ptr;
}

//-----------------------------------------------------------------------------
void RsyncPacket::swap( void* data_ptr, ui32 size_bytes )
{
   RsyncPacket::Data* header_ptr = NULL;

   if ( data_ptr && ( size_bytes >= sizeof( RsyncPacket::Data ) ) )
   {
      header_ptr = reinterpret_cast<RsyncPacket::Data*>( data_ptr );

      header_ptr->type   = ByteOrder::NetSwap( header_ptr->type );
      header_ptr->length = ByteOrder::NetSwap( header_ptr->length );
   }
}

//-----------------------------------------------------------------------------
RsyncPacketLite::RsyncPacketLite( const void* data_ptr, ui32 length )
   : valid_( false )
   , data_ptr_( data_ptr )
   , length_( length )
{
   if ( ( data_ptr_ != NULL ) && ( length_ >= sizeof( Data ) ) )
   {
      valid_ = true;
   }
}

//-----------------------------------------------------------------------------
bool RsyncPacketLite::valid() const
{
   return valid_;
}

//-----------------------------------------------------------------------------
const RsyncPacketLite::Data* RsyncPacketLite::header() const
{
   return reinterpret_cast<const Data*>( data_ptr_ );
}

//-----------------------------------------------------------------------------
const void* RsyncPacketLite::data() const
{
   if ( length_ > sizeof(Data) )
   {
      return reinterpret_cast<const ui8*>(data_ptr_) +
             sizeof(Data);
   }
   else
   {
      return NULL;
   }
}

