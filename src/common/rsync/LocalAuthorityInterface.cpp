#include "Authority.h"
#include "Log.h"
#include "LocalAuthorityInterface.h"
#include "SegmentHash.h"
#include "RsyncJob.h"
#include "FileSystemInterface.h"

#define  DEFAULT_SEGMENT_TIMEOUT_MS     (100000)

using namespace liber;
using namespace liber::rsync;

//----------------------------------------------------------------------------
LocalAuthorityInterface::
  LocalAuthorityInterface(FileSystemInterface& rFileSystemInterface)
: mnSegmentTimeoutMs(DEFAULT_SEGMENT_TIMEOUT_MS)
, mrFileSys(rFileSystemInterface)
{
}

//----------------------------------------------------------------------------
LocalAuthorityInterface::~LocalAuthorityInterface()
{
}

//----------------------------------------------------------------------------
std::ifstream& LocalAuthorityInterface::file()
{
  return mAuthFile;
}

//----------------------------------------------------------------------------
void LocalAuthorityInterface::process(RsyncJob* pJob)
{
  processJob(pJob, pJob->instructions());
}

//----------------------------------------------------------------------------
void LocalAuthorityInterface::
process(RsyncJob* pJob, InstructionReceiver& rInstructions)
{
  processJob(pJob, rInstructions);
}

//----------------------------------------------------------------------------
void LocalAuthorityInterface::
processJob(RsyncJob* pJob, InstructionReceiver& rInstructions)
{
  int lnReceived = 0;
  bool lbHashInsertDone = false;
  RsyncError lStatus = RsyncSuccess;

  pJob->report().source.authority.hashBegin.sample();
  while (lbHashInsertDone == false)
  {
    Segment* lpSegment = NULL;

    if (pJob->segments().pop(&lpSegment, mnSegmentTimeoutMs) && lpSegment)
    {
      if (lpSegment->endOfStream() == false)
      {
        lnReceived++;
        mAuthority.hash().insert(lpSegment->getWeak().checksum(), lpSegment);
      }
      else
      {
        lbHashInsertDone = true;
      }
    }
    else
    {
      log::error("AuthorityInterface::processJob - "
                 "Timed out waiting for segment (%d).\n",
                 lnReceived);
      lStatus = RsyncDestSegmentTimeout;
      lbHashInsertDone = true;
    }
  }
  pJob->report().source.authority.hashEnd.sample();

  if (lStatus == RsyncSuccess)
  {
    // Hash has been populated. Now the Authority can begin building the
    // instructions.
    if (mrFileSys.open(pJob->descriptor().getSource().path, file()))
    {
      bool lbAuthSuccess = mAuthority.process(pJob->descriptor(),
                                              file(),
                                              rInstructions,
                                              pJob->report().source);

      if (lbAuthSuccess == false)
      {
        log::error("AuthorityInterface: "
                   "Authoritative processing for %s failed.\n",
                   pJob->descriptor().getSource().path.string().c_str());
      }

      file().close();
    }
    else
    {
      log::error("AuthorityInterface: Failed to open %s\n",
                 pJob->descriptor().getSource().path.string().c_str());
      lStatus = RsyncSourceFileNotFound;
    }
  }

  if (lStatus != RsyncSuccess)
  {
    EndInstruction* lpEnd = new EndInstruction();
    lpEnd->cancel(lStatus);
    rInstructions.push(lpEnd);
  }
}

