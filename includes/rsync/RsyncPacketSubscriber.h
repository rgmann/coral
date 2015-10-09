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



#ifndef  RSYNC_PACKET_SUBSCRIBER_H
#define  RSYNC_PACKET_SUBSCRIBER_H


#include "PacketSubscriber.h"

namespace liber {
namespace rsync {

class RsyncJob;

class RsyncPacketSubscriber : public liber::netapp::PacketSubscriber {
public:

   RsyncPacketSubscriber( bool require_active_job = true );

   void setActiveJob( RsyncJob* job_ptr );
   void unsetActiveJob();

   //
   // Receive and process a packet for the specified destination. The
   // RsyncPacketSubscriber expects the first sixteen bytes to be occupied by
   // the job UUID.
   //
   bool put( liber::netapp::DestinationID destination_id, const void* data_ptr, ui32 length );


protected:

   //
   // Get a pointer to the active job.
   //
   // @precondition  The active job lock has already been acquired.
   //
   RsyncJob* activeJob();

   void disablePacketRouting();

   boost::mutex& activeJobLock();

   bool sendTo( liber::netapp::DestinationID destination_id, int type, const void* data_ptr, ui32 length );

   virtual bool processPacket( const void* data_ptr, ui32 length ) = 0;

   virtual bool validUuid( const ui8* uuid_ptr );


private:

   bool require_active_job_;

   boost::mutex lock_;

   RsyncJob* active_job_ptr_;

   bool allow_packets_;

};

}
}


#endif  // RSYNC_PACKET_SUBSCRIBER_H
