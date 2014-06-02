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
      if (mSegmentFile.open(lpJob->descriptor()))
      {
        if (mrFileSys.stage(lpJob->descriptor().getDestination(),
                            lStagePath,
                            mAssembler.outputStream()))
        {

          bool lbSuccess = mAssembler.process(lpJob->instructions(),
                                              lpJob->report().destination().assembly);
          if (lbSuccess == false)
          {
            log::error("AssemblerThread - Assembly failed for %s: %s\n",
                       lpJob->descriptor().getDestination().string().c_str(),
                       ExecutionStatus::ErrorDescription(mAssembler.status().error).c_str());
          }

          // Close the segment file.
          mSegmentFile.close();

          // If assembly completed successfully,
          // swap the stage file and destination file.
          if (lbSuccess)
          {
            if (!mrFileSys.swap(lStagePath, lpJob->descriptor().getDestination()))
            {
              log::error("AssemblerThread - "
                         "Failed to swap stage file and destination file.\n");
            }
          }
        }
        else
        {
          mSegmentFile.close();

          log::error("AssemblerThread - Failed to open stage file for %s\n",
                     lpJob->descriptor().getDestination().string().c_str());
        }
      }
      else
      {
        log::error("AssemblerThread - Failed to open segment access file.\n");
      }

      // When assembly is complete (regardless of whether is was successful),
      // signal that the RSYNC job is done.
      lpJob->signalDone();
    }
  }
}

