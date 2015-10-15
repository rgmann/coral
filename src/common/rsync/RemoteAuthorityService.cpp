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



#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid_io.hpp>
#include "Log.h"
#include "RsyncJob.h"
#include "RsyncPacket.h"
#include "Segment.h"
#include "RsyncQueryResponse.h"
#include "RsyncPacketRouter.h"
#include "RemoteAuthorityService.h"

using namespace coral;
using namespace coral::rsync;
using namespace coral::netapp;


//-----------------------------------------------------------------------------
RemoteAuthorityService::RemoteAuthorityService()
:  RsyncPacketSubscriber( true )
,  user_handler_ptr_( NULL )
,  request_id_( RsyncPacket::RsyncAuthorityInterface )
{
}

//-----------------------------------------------------------------------------
RemoteAuthorityService::~RemoteAuthorityService()
{
}

//-----------------------------------------------------------------------------
void RemoteAuthorityService::setRequestID( int request_id )
{
   request_id_ = request_id;
}

//-----------------------------------------------------------------------------
void RemoteAuthorityService::process( RsyncJob* job_ptr )
{
   bool subscribed = job_ptr->packetRouter().subscribe(
      RsyncPacket::RsyncAuthorityService,
      this,
      coral::netapp::kSubscriberModeReadWrite
   );

   if ( subscribed )
   {
      setActiveJob( job_ptr );

      RsyncQueryResponse response( job_ptr->descriptor().uuid(), kRsyncSuccess );

      std::string packet_data = response.serialize();
      sendTo(
         RsyncPacket::RsyncAuthorityInterface,
         RsyncPacket::RsyncRemoteAuthAcknowledgment,
         packet_data.data(),
         packet_data.size() );

      authority_.process( job_ptr, *this );

      packet_data = job_ptr->report().source.serialize();
      bool auth_report_send_success = sendTo(
         RsyncPacket::RsyncAuthorityInterface,
         RsyncPacket::RsyncAuthorityReport,
         packet_data.data(),
         packet_data.size() );

      if ( auth_report_send_success == false )
      {
         log::error("SendReportHook - Failed to send report to %d\n", request_id_);
      }

      unsetActiveJob();

      job_ptr->packetRouter().unsubscribe(
         RsyncPacket::RsyncAuthorityService,
         this
      );
   }
}

//-----------------------------------------------------------------------------
bool RemoteAuthorityService::processPacket( const void* data_ptr, ui32 length )
{
   bool success = false;
   RsyncPacketLite packet( data_ptr, length );

   if ( packet.valid() )
   {
      switch ( packet.header()->type )
      {
         case RsyncPacket::RsyncSegment:
            pushSegment( packet.data(), packet.header()->length );
            success = true;
            break;

         default:
            log::debug("RemoteAuthorityService::put - invalid packet type=%d\n",
                      packet.header()->type);
            break;
      }
   }
   else
   {
      log::debug("RemoteAuthorityService::put - failed to unpack packet\n");
   }

   return success;
}

//-----------------------------------------------------------------------------
void RemoteAuthorityService::call( RawInstructionPtr instruction_ptr )
{
   if ( instruction_ptr )
   {
      sendTo(
         RsyncPacket::RsyncAuthorityInterface,
         RsyncPacket::RsyncInstruction,
         instruction_ptr->data(),
         instruction_ptr->length() );
   }
}

//-----------------------------------------------------------------------------
void RemoteAuthorityService::pushSegment(const void* pData, ui32 nLength)
{
   Segment* segment_ptr = new Segment();

   if ( segment_ptr->deserialize( (const char*)pData, nLength ) )
   {
      activeJob()->segments().push( segment_ptr );
   }
   else
   {
      log::error("Failed to deserialize segment.\n");
      delete segment_ptr;
   }
}
