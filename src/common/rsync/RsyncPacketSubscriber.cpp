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



#include <string.h>
#include "Log.h"
#include "RsyncJob.h"
#include "RsyncPacket.h"
#include "RsyncPacketSubscriber.h"

#define  RSYNC_JOB_UUID_LENGTH      ( 16 )

using namespace coral;
using namespace coral::rsync;
using namespace coral::netapp;

//-----------------------------------------------------------------------------
RsyncPacketSubscriber::RsyncPacketSubscriber( bool require_active_job )
:  require_active_job_( require_active_job )
,  active_job_ptr_( NULL )
,  allow_packets_( true )
{
}

//-----------------------------------------------------------------------------
void RsyncPacketSubscriber::setActiveJob( RsyncJob* job_ptr )
{
   boost::mutex::scoped_lock guard( lock_ );
   active_job_ptr_ = job_ptr;
}

//-----------------------------------------------------------------------------
void RsyncPacketSubscriber::unsetActiveJob()
{
   boost::mutex::scoped_lock guard( lock_ );
   active_job_ptr_ = NULL;
}

//-----------------------------------------------------------------------------
boost::mutex& RsyncPacketSubscriber::activeJobLock()
{
   return lock_;
}

//-----------------------------------------------------------------------------
bool RsyncPacketSubscriber::validUuid( const ui8* uuid_ptr )
{
   bool valid_uuid = true;

   if ( require_active_job_ )
   {
      boost::mutex::scoped_lock guard( lock_ );

      if ( active_job_ptr_ )
      {
         const boost::uuids::uuid& job_id = active_job_ptr_->descriptor().uuid();

         for ( ui32 pos = 0; pos < RSYNC_JOB_UUID_LENGTH; ++pos )
         {
            if ( job_id.data[ pos ] != uuid_ptr[ pos ] )
            {
               valid_uuid = false;
               break;
            }
         }
      }
      else
      {
         log::error("%s - Active job required to send packet.\n",
            BOOST_CURRENT_FUNCTION );

         valid_uuid = false;
      }
   }

   return valid_uuid;
}

//-----------------------------------------------------------------------------
bool RsyncPacketSubscriber::put(
   DestinationID  destination_id,
   const void*    data_ptr,
   ui32           length
)
{
   bool success = false;

   if ( allow_packets_ )
   {
      if ( data_ptr && ( length > RSYNC_JOB_UUID_LENGTH ) )
      {
         const ui8*  uuid_ptr    = reinterpret_cast<const ui8*>(data_ptr);
         const void* payload_ptr = uuid_ptr + RSYNC_JOB_UUID_LENGTH;

         if ( validUuid( uuid_ptr ) )
         {
            success = processPacket(
               payload_ptr,
               length - RSYNC_JOB_UUID_LENGTH );
         }
         else
         {
            log::error(
               "RsyncPacketSubscriber::put: "
               "ERROR - Invalid UUID cannot deliver packet\n");
         }
      }
   }

   return success;
}

//-----------------------------------------------------------------------------
RsyncJob* RsyncPacketSubscriber::activeJob()
{
   // boost::mutex::scoped_lock guard( lock_ );
   return active_job_ptr_;
}

//-----------------------------------------------------------------------------
void RsyncPacketSubscriber::disablePacketRouting()
{
   allow_packets_ = false;
}

//-----------------------------------------------------------------------------
bool RsyncPacketSubscriber::sendTo(
   DestinationID  destination_id,
   int            type,
   const void*    data_ptr,
   ui32           length
)
{
   bool success = false;

   RsyncPacket* packet_ptr = new (std::nothrow) RsyncPacket(
      type,
      length,
      data_ptr
   );

   if ( packet_ptr )
   {

      bool uuid_success = true;
      bool valid_uuid   = false;

      boost::uuids::uuid job_id;

      {
         boost::mutex::scoped_lock guard( lock_ );
         
         if ( active_job_ptr_ )
         {
            valid_uuid = true;
            job_id     = active_job_ptr_->descriptor().uuid();
         }
      }

      if ( require_active_job_ || valid_uuid )
      {
         if ( valid_uuid )
         {
            memcpy(
               packet_ptr->data()->job_id, job_id.data,
               RSYNC_JOB_UUID_LENGTH
            );
         }
         else
         {
            log::error("%s - Active job required to send packet.\n",
               BOOST_CURRENT_FUNCTION );

            uuid_success = false;
         }
      }

      if ( uuid_success )
      {
         success = PacketSubscriber::sendTo( destination_id, packet_ptr );
      }

      //
      // If we failed to send the packet, deallocate the packet now.
      if ( success == false )
      {
         delete packet_ptr;
         packet_ptr = NULL;
      }
   }
   else
   {
      log::error("%s - Failed to allocate RsyncPacket.\n",
         BOOST_CURRENT_FUNCTION );
   }

   return success;
}
