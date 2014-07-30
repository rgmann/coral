#include "Log.h"
#include "RsyncJob.h"
#include "Segmenter.h"
#include "SegmenterThread.h"

using namespace liber;
using namespace liber::rsync;
using namespace liber::concurrency;

//----------------------------------------------------------------------------
SegmenterThread::SegmenterThread(FileSystemInterface& rFileSystemInterface)
: IThread("SegmenterThread")
, mrFileSys(rFileSystemInterface)
, mSegmenter(Segmenter::FullStride)
{
  mJobQueue.initialize();
}

//----------------------------------------------------------------------------
SegmenterThread::~SegmenterThread()
{
}

//----------------------------------------------------------------------------
void SegmenterThread::addJob(RsyncJob* pJob)
{
  mJobQueue.push(pJob);
}

//----------------------------------------------------------------------------
void SegmenterThread::run(const bool& bShutdown)
{
  while (!bShutdown)
  {
    RsyncJob* lpJob = NULL;

    if (mJobQueue.pop(lpJob) && lpJob)
    {
      std::ifstream lInputStream;

      if (mrFileSys.open(lpJob->descriptor().getDestination().path, lInputStream))
      {
        bool lbSuccess = mSegmenter.process(lInputStream,
                                            lpJob->segments(),
                                            lpJob->descriptor().getSegmentSize(),
                                            lpJob->report().destination.segmentation);
        if (lbSuccess == false)
        {
          log::error("SegmenterThread: %s failed\n",
                     lpJob->descriptor().getDestination().path.string().c_str());
        }

        lInputStream.close();
      }
      else
      {
        log::debug("SegmenterThread: Failed to open %s\n.",
                     lpJob->descriptor().getDestination().path.string().c_str());
      }
    }
  }
}

