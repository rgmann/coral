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



#ifndef PACKET_RELAY_H
#define PACKET_RELAY_H


#include "PacketReceiverHook.h"
#include "PacketRouter.h"
#include "PacketSubscriber.h"


namespace coral {
namespace netapp {

// The RelayReceiverHook class simply receives PacketContainers from
// subscribers, translates/repackages them, and then "relays" them to
// the supplied PacketReceiver.
class RelayReceiverHook : public PacketReceiverHook {
public:

  RelayReceiverHook();
  virtual ~RelayReceiverHook();

  bool registerReceiver( const RegisteredRouter& receiver );
  void unregisterReceiver( DestinationID destination_id );

protected:

  bool call(PacketContainer* pContainer);

  virtual PacketContainer* translate( DestinationID destination_id, PacketContainer* pContainer );

private:

  // PacketReceiver* receiver_ptr_;
  RegisteredRouterList routers_;
};


// The PacketRelay class allows a multiple-subscriber application to share a
// single subscriber ID. All packets leaving the subscribers will become the
// payload of packet with the PacketRelay's subscriber ID. Incomming packets
// are multiplexed to the subscribers register with the PacketRelay.
class PacketRelay : public PacketRouter, public PacketSubscriber {
public:

  /**
   * The PacketRelay depends on the ReceiverHook to offload reformatting and
   * transmission of outgoing packets to the subscriber's thread context.
   * Any number of PacketRelay subscriber's can transmit packets
   * simultaneously.
   */
  explicit PacketRelay(RelayReceiverHook& rInReceiver);

  virtual ~PacketRelay();

  bool put( DestinationID destination_id, const void* data_ptr, ui32 length );

protected:

   struct RelayInfo {
      DestinationID destination_id;
      const void* data_ptr;
      ui32 length;
      RelayInfo()
      : destination_id( 0 )
      , data_ptr( NULL )
      , length( 0 )
      {}
      RelayInfo( DestinationID destination_id, const void* data_ptr, ui32 length )
      :  destination_id( destination_id )
      ,  data_ptr( data_ptr )
      ,  length( length )
      {}
   };

   /**
    * Pure virtual method to extract an encapsulated packet and return a
    * routable PacketContainer. This method assumes that the received
    * represents a packet parsable by this subscriber. The packet header
    * should indicate the PacketRelay subscriber that should receiver the packet
    * payload. The method is virtual to allow flexibility in how the packet
    * header is defined.
    */
    virtual RelayInfo* extract( const RelayInfo& input ) = 0;

   virtual PacketReceiver* setReceiver( RegisteredRouter& router_info );

private:

   RelayReceiverHook& relay_receiver_;
};


} // End namespace netapp
} // End namespace coral

#endif // PACKET_RELAY_H
