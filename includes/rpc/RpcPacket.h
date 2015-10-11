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



#ifndef RPC_PACKET_H
#define RPC_PACKET_H

#include "GenericPacket.h"
#include "RpcObject.h"

namespace liber {
namespace rpc {

class RpcPacket : public liber::netapp::GenericPacket {
public:

   enum Type
   {
      // TODO: add types
   };

   static const ui32 RpcMarkerSize = 7;
   struct __attribute ((__packed__)) Data
   {
      char marker[RpcMarkerSize];
      ui32 rpcId;
      ui32 length;
      ui32 format;
      ui32 encoding;
   };

   RpcPacket();
   
   RpcPacket(const RpcObject &object); 
   
   bool  getObject(RpcObject &object) const;

   // bool  unpack(const void* pPkt, ui32 nSizeBytes);
   bool  allocate(const void* pPkt, ui32 nSizeBytes);
   
   Data* const data() const;

   void swap(void* pData, ui32 nSizeBytes);
   
private:

   typedef liber::netapp::GenericPacket inherited;

};

}}

#endif // RPC_PACKET_H
