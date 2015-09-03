#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid_io.hpp>
#include "Log.h"
#include "RsyncJob.h"
#include "RsyncPacket.h"
#include "Segment.h"
#include "RsyncQueryResponse.h"
#include "RsyncPacketRouter.h"
#include "RemoteAuthorityService.h"

using namespace liber;
using namespace liber::rsync;
using namespace liber::netapp;


//-----------------------------------------------------------------------------
RemoteAuthorityService::RemoteAuthorityService()
: RsyncPacketSubscriber( true )
, mpUserHandler(NULL)
, mRequestID(RsyncPacket::RsyncAuthorityInterface)
{
}

//-----------------------------------------------------------------------------
RemoteAuthorityService::~RemoteAuthorityService()
{
}

//-----------------------------------------------------------------------------
void RemoteAuthorityService::setRequestID(int requestID)
{
  mRequestID = requestID;
}

//-----------------------------------------------------------------------------
void RemoteAuthorityService::process( RsyncJob* job_ptr )
{
  job_ptr->packetRouter().subscribe(
    RsyncPacket::RsyncAuthorityService, this, liber::netapp::kSubscriberModeReadWrite );

  // {
  //   boost::mutex::scoped_lock guard( active_job_lock_ );
  //   active_job_ = job_ptr;
  // }
  setActiveJob( job_ptr );

  RsyncQueryResponse response( job_ptr->descriptor().uuid(), kRsyncSuccess );

  // sendTo( mRequestID, new (std::nothrow) RsyncPacket(
  //   RsyncPacket::RsyncRemoteAuthAcknowledgment,
  //   response.serialize()
  // ));
  std::string packet_data = response.serialize();
  sendTo(
    RsyncPacket::RsyncAuthorityInterface,
    RsyncPacket::RsyncRemoteAuthAcknowledgment,
    packet_data.data(),
    packet_data.size() );

  authority_.process( job_ptr, *this );
  // liber::log::debug("RemoteAuthorityService::process: FINISHED PROCESSING\n");

  // JobReport& rReport = job_ptr->report();

  // bool auth_report_send_success = sendTo( mRequestID, new RsyncPacket(
  //   RsyncPacket::RsyncAuthorityReport,
  //   job_ptr->report().source.serialize()
  // ));
  packet_data = job_ptr->report().source.serialize();
  bool auth_report_send_success = sendTo(
    RsyncPacket::RsyncAuthorityInterface,
    RsyncPacket::RsyncAuthorityReport,
    packet_data.data(),
    packet_data.size() );

  if ( auth_report_send_success == false )
  {
    log::error("SendReportHook - Failed to send report to %d\n", mRequestID);
  }

  // {
  //   boost::mutex::scoped_lock guard( active_job_lock_ );
  //   active_job_ = NULL;
  // }
  unsetActiveJob();

  job_ptr->packetRouter().unsubscribe( RsyncPacket::RsyncAuthorityService, this );
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
void RemoteAuthorityService::call( InstructionRaw* instruction_ptr )
{
   if ( instruction_ptr )
   {

      // std::string packet_data = container_ptr->stream().stream.str();
      // log::status("RemoteAuthorityService::call: %d, %d\n", instruction_ptr->length(), instruction_ptr->payload_length());
      // instruction_ptr->dump();
      sendTo(
         RsyncPacket::RsyncAuthorityInterface,
         RsyncPacket::RsyncInstruction,
         instruction_ptr->data(),
         instruction_ptr->length() );

      delete instruction_ptr;
   }
}

//-----------------------------------------------------------------------------
void RemoteAuthorityService::pushSegment(const void* pData, ui32 nLength)
{
  // boost::mutex::scoped_lock guard( active_job_lock_ );

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
