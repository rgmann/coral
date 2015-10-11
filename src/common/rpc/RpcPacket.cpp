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

static const char MarkerData[RpcPacket::RpcMarkerSize] = {'r', 'p', 'c',
                                             'm', 's', 'g', '\0'};

//------------------------------------------------------------------------------
RpcPacket::RpcPacket()
   : GenericPacket( sizeof(RpcPacket::Data), 0 )//sizeof(RpcPacket::Data)
{
}

//------------------------------------------------------------------------------
RpcPacket::RpcPacket(const RpcObject &object)
   : GenericPacket( sizeof(RpcPacket::Data), 0 )//sizeof(RpcPacket::Data)
{
   std::string serialized_object = object.serialize();

   if ( serialized_object.empty() )
   {
      log::error("RpcPacket - Empty RpcObject. No allocation performed.\n");
   }
   else
   {
     // if (allocate( sizeof(Data), serialized_object.size()))
      if (GenericPacket::allocate( sizeof(Data) + serialized_object.size()))
      {
         memcpy(data()->marker, MarkerData, RpcMarkerSize);
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
   return reinterpret_cast<Data* const>(basePtr());
}

//------------------------------------------------------------------------------
// bool RpcPacket::unpack(const void* pPkt, ui32 nSizeBytes)
bool RpcPacket::allocate(const void* pPkt, ui32 nSizeBytes)
{
   if (!inherited::allocate(pPkt, nSizeBytes))
   {
      log::error("RpcPacket::unpack: inherited unpack failed.\n");
      return false;
   }
   
   // Verify that the packet is at least large enough to contain the
   // RsyncAssemblyInstr data segment and all preceding data segments.
   // if (nSizeBytes < sizeof(Data))
   // {
   //    log::error("RpcPacket::unpack: too small\n");
   //    return false;
   // }

   if (strncmp(data()->marker, MarkerData, RpcPacket::RpcMarkerSize) != 0)
   {
      log::error("RpcPacket::unpack: Invalid marker\n");
      return false;
   }
       
   // Validate the size of the packet against the indicated payload size.
   if ((nSizeBytes - sizeof(Data)) != data()->length)
   {
      log::error("RpcPacket::unpack: size mismatch - size = %u, exp = %u\n",
             (nSizeBytes - sizeof(Data)), data()->length);
      return false;
   }
       
   return true;
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


