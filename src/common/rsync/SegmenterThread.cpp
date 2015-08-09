#include "Log.h"
#include "RsyncJob.h"
#include "Segmenter.h"
#include "FileSystemInterface.h"
#include "SegmenterThread.h"

using namespace liber;
using namespace liber::rsync;
using namespace liber::concurrency;

//----------------------------------------------------------------------------
SegmenterThread::SegmenterThread( JobAgentPairQueue& queue )
: IThread("SegmenterThread")
, job_queue_( queue )
{
}

//----------------------------------------------------------------------------
SegmenterThread::~SegmenterThread()
{
}

//----------------------------------------------------------------------------
void SegmenterThread::run(const bool& shutdown )
{
   while ( shutdown == false )
   {
      JobAgentPair job_agent_pair;

      if ( job_queue_.pop( job_agent_pair ) )
      {
         JobAgent* agent_ptr = job_agent_pair.first;
         RsyncJob* job_ptr = job_agent_pair.second;

         if ( agent_ptr && job_ptr )
         {
            std::ifstream input_file;

            bool segment_success = job_ptr->fileSystem().open(
               job_ptr->descriptor().getDestination().path(),
               input_file
            );

            if ( segment_success )
            {
               segment_success = Segmenter::processFullStride(
                  input_file,
                  job_ptr->segments(),
                  job_ptr->descriptor().getSegmentSize(),
                  job_ptr->report().destination.segmentation
               );

               if ( segment_success == false )
               {
                  log::error("SegmenterThread: %s failed\n",
                             job_ptr->descriptor().getDestination().path().string().c_str());
               }

               input_file.close();
            }
            else
            {
               log::debug("SegmenterThread: Failed to open %s\n.",
                          job_ptr->descriptor().getDestination().path().string().c_str());
            }

            job_ptr->signalSegmentationDone();
         }
      }
   }
}

