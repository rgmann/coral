#include "Log.h"
#include "RsyncJob.h"
#include "SegmentFile.h"
#include "JobDescriptor.h"
#include "FileSystemInterface.h"
#include "AssemblerThread.h"

using namespace liber;
using namespace liber::concurrency;
using namespace liber::rsync;

//----------------------------------------------------------------------------
AssemblerThread::AssemblerThread(FileSystemInterface& rFileSystemInterface)
: IThread("AssemblerThread")
, mrFileSys(rFileSystemInterface)
, mSegmentFile(rFileSystemInterface)
, mAssembler(mSegmentFile)
{
  mJobQueue.initialize();
}

//----------------------------------------------------------------------------
AssemblerThread::~AssemblerThread()
{
}

//----------------------------------------------------------------------------
void AssemblerThread::addJob(RsyncJob* pJob)
{
  mJobQueue.push(pJob);
}

//----------------------------------------------------------------------------
void AssemblerThread::run(const bool& bShutdown)
{
  boost::filesystem::path lStagePath;

  while (!bShutdown)
  {
    RsyncJob* lpJob = NULL;

    if (mJobQueue.pop(lpJob) && lpJob)
    {
      mSegmentFile.open(lpJob->descriptor());

      mrFileSys.stage(lpJob->descriptor().getDestination(), lStagePath, mAssembler.outputStream());

      if (mAssembler.process(lpJob->instructions(),
                             lpJob->report().destination().assembly) == false)
      {
        log::error("Assembly failed for %s: %s\n",
                   lpJob->descriptor().getDestination().string().c_str(),
                   ExecutionStatus::ErrorDescription(mAssembler.status().error).c_str());
      }
      else
      {
        mSegmentFile.close();
        mrFileSys.swap(lStagePath, lpJob->descriptor().getDestination());
      }

      // When assembly is complete (regardless of whether is was successful),
      // signal that the RSYNC job is done.
      lpJob->signalDone();
    }
  }
}

