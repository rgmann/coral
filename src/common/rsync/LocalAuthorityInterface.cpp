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
  RsyncError lStatus = RsyncSuccess;

  pJob->report().source().authority.hashBegin.sample();
  while (1)
  {
    Segment* lpSegment = NULL;

    if (pJob->segments().pop(&lpSegment, mnSegmentTimeoutMs) && lpSegment && (lpSegment->endOfStream() == false))
    {
      lnReceived++;
      //mHash.insert(lpSegment->getWeak().checksum(), lpSegment);
      mAuthority.hash().insert(lpSegment->getWeak().checksum(), lpSegment);
    }
    else
    {
      if (lpSegment == NULL)
      {
        // TODO: Best way to handle NULL pointer?
        log::error("AuthorityInterface: Timed out waiting for segment (%d).\n",
                   lnReceived);
        lStatus = RsyncDestSegmentTimeout;
      }
      else if (lpSegment->endOfStream())
      {
        log::status("AuthorityInterface: END OF STREAM\n");
      }

      break;
    }
  }
  pJob->report().source().authority.hashEnd.sample();

  if (lStatus == RsyncSuccess)
  {
    // Hash has been populated. Now the Authority can begin building the
    // instructions.
//    Authority* lpAuthority = new Authority(pJob->descriptor(), mHash, pJob->instructions());

    if (mrFileSys.open(pJob->descriptor().getSource(), file()))
    {
      //if (lpAuthority->process(file(), pJob->report().source()))
      if (mAuthority.process(pJob->descriptor(), file(), rInstructions, pJob->report().source()))
      {
        log::debug("AuthorityInterface: Completed instructions for %s\n",
                   pJob->descriptor().getSource().string().c_str());
      }
      else
      {
        log::error("AuthorityInterface: Authoritative processing for %s failed.\n",
                   pJob->descriptor().getSource().string().c_str());
      }

      file().close();
    }
    else
    {
      log::error("AuthorityInterface: Failed to open %s\n",
                 pJob->descriptor().getSource().string().c_str());
    }

//    delete lpAuthority;
  }
  else
  {
    EndInstruction* lpEnd = new EndInstruction();
    lpEnd->cancel(lStatus);
    //pJob->instructions().push(lpEnd);
    rInstructions.push(lpEnd);
  }

//  SegmentDestructor destructor;
//  mHash.clear(destructor);
}

