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



#ifndef PACKET_ROUTER_H
#define PACKET_ROUTER_H

#include <map>
#include <boost/thread/mutex.hpp>
#include "PacketReceiver.h"
#include "PacketSubscriber.h"

namespace coral {
namespace netapp {

class PacketContainer;

//
// A Channel routes all associated packets to all subscribed endpoints
// (subscribers).
//
//
class Destination {
public:

   Destination( DestinationID destination_id ) : destination_id_( destination_id ){};

   DestinationID id() const { return destination_id_; }

   SubscriberList& subscribers(){ return subscribers_; }

   boost::mutex& lock(){ return lock_; }

private:

   DestinationID destination_id_;

   SubscriberList subscribers_;

   boost::mutex lock_;
};
typedef std::map<DestinationID,Destination*> DestinationTable;
typedef DestinationTable::iterator  DestinationIterator;

class PacketRouter {
public:

   PacketRouter(PacketReceiver* pReceiver);
   virtual ~PacketRouter();

   bool subscribe(
      DestinationID destination_id,
      PacketSubscriber* subscriber_ptr,
      SubscriberMode mode = kSubscriberModeReadWrite );
   bool unsubscribe( DestinationID destination_id, PacketSubscriber* subscriber_ptr );

   i32 count( DestinationID destination_id );
   bool empty();

   bool publish( DestinationID destination_id, const void* data_ptr, ui32 length );


protected:

   virtual bool allowPublish(
      DestinationID destination_id,
      PacketSubscriber* subscriber_ptr,
      const void* data_ptr, ui32 length ) { return true; }


  //
  PacketReceiver* receiver_ptr_;


private:

  //
  boost::mutex table_lock_;

  //
  DestinationTable destination_table_;
};

}}

#endif // PACKET_ROUTER_H

