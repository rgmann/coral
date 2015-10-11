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
#include <string.h>
#include <iostream>
#include "Log.h"
#include "RpcPacket.h"
#include "ByteOrder.h"

using namespace liber;
using namespace liber::rpc;
using namespace liber::netapp;
using namespace liber::net;

static const char MarkerData[ RpcPacket::kRpcMarkerSize ] =
   { 'r', 'p', 'c', 'm', 's', 'g', '\0' };

//------------------------------------------------------------------------------
RpcPacket::RpcPacket()
   : GenericPacket( sizeof(RpcPacket::Data), 0 )
{
}

//------------------------------------------------------------------------------
RpcPacket::RpcPacket(const RpcObject &object)
   : GenericPacket( sizeof(RpcPacket::Data), 0 )
{
   std::string serialized_object = object.serialize();

   if ( serialized_object.empty() )
   {
      log::error("RpcPacket - Empty RpcObject. No allocation performed.\n");
   }
   else
   {
      if ( GenericPacket::allocate( serialized_object.size() ) )
      {
         memcpy( data()->marker, MarkerData, kRpcMarkerSize );
         data()->length = serialized_object.size();

         memcpy( dataPtr(), serialized_object.data(), serialized_object.size() );
      }
      else
      {
         log::error("RpcPacket - Allocation failed.\n");
      }
   }
}

//------------------------------------------------------------------------------
bool RpcPacket::getObject( RpcObject &object ) const
{
   bool deserialize_success = false;
   
   if ( isAllocated() )
   {
      deserialize_success =
         object.deserialize( (char*)dataPtr(), data()->length );

      if ( deserialize_success == false )
      {
         log::error("RpcPacket::getObject: Failed to deserialize.\n");
      }
   }
   
   return deserialize_success;
}

//------------------------------------------------------------------------------
RpcPacket::Data* const RpcPacket::data() const
{
   return reinterpret_cast<Data* const>( basePtr() );
}

//------------------------------------------------------------------------------
bool RpcPacket::allocate( const void* data_ptr, ui32 size_bytes )
{
   bool success = false;

   if ( inherited::allocate( data_ptr, size_bytes ) )
   {
      if ( strncmp( data()->marker, MarkerData, kRpcMarkerSize ) == 0)
      {
         // Validate the size of the packet against the indicated payload size.
         if ( ( size_bytes - sizeof( Data ) ) == data()->length )
         {
            success = true;
         }
         else
         {
            log::error(
               "RpcPacket::unpack: size mismatch - size = %u, exp = %u\n",
               ( size_bytes - sizeof( Data ) ), data()->length);
         }
      }
      else
      {
         log::error("RpcPacket::unpack: Invalid marker\n");
      }
   }

   return success;
}

//------------------------------------------------------------------------------
void RpcPacket::swap(void* pData, ui32 nSizeBytes)
{
  if (pData && (nSizeBytes >=  sizeof(RpcPacket::Data)))
  {
    RpcPacket::Data* lpData = reinterpret_cast<RpcPacket::Data*>(pData);

    liber::netapp::swapInPlace(lpData->rpcId);
    liber::netapp::swapInPlace(lpData->length);
    liber::netapp::swapInPlace(lpData->format);
    liber::netapp::swapInPlace(lpData->encoding);
  }
}
