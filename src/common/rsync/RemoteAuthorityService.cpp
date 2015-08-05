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
: PacketSubscriber()
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
  job_ptr->packetRouter().addSubscriber( RsyncPacket::RsyncAuthorityService, this );

  {
    boost::mutex::scoped_lock guard( active_job_lock_ );
    active_job_ = job_ptr;
  }

  RsyncQueryResponse response( job_ptr->descriptor().uuid(), RsyncSuccess );

  sendPacketTo( mRequestID, new (std::nothrow) RsyncPacket(
    RsyncPacket::RsyncRemoteAuthAcknowledgment,
    response.serialize()
  ));
  // liber::log::debug("RemoteAuthorityService::process: SENT AUTH ACK\n");

  authority_.process( job_ptr, *this );
  // liber::log::debug("RemoteAuthorityService::process: FINISHED PROCESSING\n");

  // JobReport& rReport = job_ptr->report();

  bool auth_report_send_success = sendPacketTo( mRequestID, new RsyncPacket(
    RsyncPacket::RsyncAuthorityReport,
    job_ptr->report().source.serialize()
  ));
  // liber::log::debug("RemoteAuthorityService::process: SENT REPORT\n");

  if ( auth_report_send_success == false )
  {
    log::error("SendReportHook - Failed to send report to %d\n", mRequestID);
  }

  {
    boost::mutex::scoped_lock guard( active_job_lock_ );
    active_job_ = NULL;
  }

  job_ptr->packetRouter().removeSubscriber( RsyncPacket::RsyncAuthorityService );
}

//-----------------------------------------------------------------------------
bool RemoteAuthorityService::put(const char* pData, ui32 nLength)
{
  bool lbSuccess = false;
  RsyncPacket packet;

  if ( packet.unpack(pData, nLength) )
  {
    switch ( packet.data()->type )
    {
      case RsyncPacket::RsyncSegment:
        pushSegment( packet.dataPtr(), packet.data()->length);
        lbSuccess = true;
        break;

      default:
        log::debug("RemoteAuthorityService::put - invalid packet type=%d\n",
                   packet.data()->type);
        break;
    }
  }
  else
  {
    log::debug("RemoteAuthorityService::put - failed to unpack packet\n");
  }

  return lbSuccess;
}

//-----------------------------------------------------------------------------
void RemoteAuthorityService::call(Instruction* pInstruction)
{
  if (pInstruction)
  {
    sendPacketTo( mRequestID, new RsyncPacket(
      RsyncPacket::RsyncInstruction,
      InstructionFactory::Serialize(pInstruction)
    ));

    delete pInstruction;
  }
}

//-----------------------------------------------------------------------------
void RemoteAuthorityService::pushSegment(const void* pData, ui32 nLength)
{
  boost::mutex::scoped_lock guard( active_job_lock_ );

  Segment* segment_ptr = new Segment();
  if ( segment_ptr->deserialize( (const char*)pData, nLength ) )
  {
    active_job_->segments().push( segment_ptr );
  }
  else
  {
    log::error("Failed to deserialize segment.\n");
    delete segment_ptr;
  }
}
