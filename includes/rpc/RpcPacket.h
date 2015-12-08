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

namespace coral {
namespace rpc {

///
/// The RpcPacket is the netapp transport container for RpcObject requests and
/// responses.
///
class RpcPacket : public coral::netapp::GenericPacket {
public:
   
   struct Data {
      // Payload length in bytes
      ui32 length;
      ui32 format;
      ui32 encoding;
   };

   ///
   /// Construct an empty packet. The packet will be allocated to the size of
   /// the Data structure.
   ///
   RpcPacket();

   ///
   /// Construct a packet to transport the specified object.
   ///
   /// @param  object  Request or response object
   ///
   RpcPacket( const RpcObject& object ); 

   ///
   /// Retrieve a deserialized request or response object from the payload of
   /// the packet.
   ///
   /// @param  object  RpcObject to deserialize from the payload of this
   ///                 packet.
   /// @return bool    True if the object was successfully deserialized; false
   ///                 otherwise
   ///
   bool getObject( RpcObject& object ) const;

   ///
   /// Allocate the packet from the specified byte buffer. This method
   /// implements the GenericPacket interface.
   ///
   /// @param  data_ptr   Pointer to data buffer
   /// @param  size_bytes Size of buffer in bytes
   /// @return bool       True if the packet is successfully allocated from the
   ///                    buffer; false otherwise
   ///
   bool allocate( const void* data_ptr, ui32 size_bytes );
   
   ///
   /// Get a pointer to the packet header. If the packet has not been
   /// allocated, NULL is returned.
   ///
   /// @return Data*  Pointer to packet header
   ///
   Data* const data() const;


protected:

   ///
   /// Swap endianness of packet fields
   ///
   void swap( void* data_ptr, ui32 size_bytes );


private:

   typedef coral::netapp::GenericPacket inherited;

};

} // end namespace rpc
} // end namespace coral

#endif // RPC_PACKET_H
