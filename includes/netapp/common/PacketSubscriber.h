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



#ifndef PACKET_SUBSCRIBER_H
#define PACKET_SUBSCRIBER_H

#include <map>
#include <list>
#include <boost/thread/mutex.hpp>
#include "PacketReceiver.h"

namespace coral {
namespace netapp {

class GenericPacket;

enum SubscriberMode {
   kSubscriberModeReadOnly,
   kSubscriberModeReadWrite
};

struct RegisteredRouter {
   DestinationID     destination_id;
   SubscriberMode    mode;
   PacketReceiver*   receiver_ptr;

   RegisteredRouter() : receiver_ptr(NULL)
   {
   }
};

typedef  std::list<RegisteredRouter> RegisteredRouterList;
typedef  std::map<DestinationID,RegisteredRouter> RegisteredRouterMap;

class PacketSubscriber {
public:

   enum { kInvalidSubscriber = -1 };

   PacketSubscriber();
   virtual ~PacketSubscriber();

   virtual bool registerRouter( DestinationID destination_id, PacketReceiver* receiver_ptr, SubscriberMode mode );
   bool unregisterRouter( DestinationID destination_id );

   bool isSubscribed() const;

   /**
    * Synchronously process a packet.  If PacketApp does not perform any
    * synchronous packet processing, simply override with an implementation
    * that does nothing.
    */
   virtual bool put( DestinationID destination_id, const void* pData, ui32 length ) = 0;

   //
   // Send a packet to the specified destination channel
   //
   //
   //
   bool sendTo( DestinationID destination_id, GenericPacket* pPacket);


protected:

   virtual PacketReceiver* setReceiver( RegisteredRouter& router_info );


private:

   RegisteredRouterMap routers_;

   mutable boost::mutex subscribe_lock_;
};

typedef std::list<PacketSubscriber*>  SubscriberList;
typedef SubscriberList::iterator      SubscriberIterator;

} // End namespace netapp
} // End namespace coral

#endif // PACKET_SUBSCRIBER_H

