#include "Log.h"
#include "RsyncJob.h"
#include "Segmenter.h"
#include "SegmenterThread.h"

using namespace liber;
using namespace liber::rsync;
using namespace liber::concurrency;

//----------------------------------------------------------------------------
SegmenterThread::SegmenterThread( FileSystemInterface& file_sys_interface )
: IThread("SegmenterThread")
, file_sys_interface_( file_sys_interface )
{
  job_queue_.initialize();
}

//----------------------------------------------------------------------------
SegmenterThread::~SegmenterThread()
{
}

//----------------------------------------------------------------------------
void SegmenterThread::addJob( RsyncJob* job_ptr )
{
  job_queue_.push( job_ptr );
}

//----------------------------------------------------------------------------
void SegmenterThread::run(const bool& bShutdown)
{
  while ( !bShutdown )
  {
    RsyncJob* job_ptr = NULL;

    if ( job_queue_.pop( job_ptr ) && job_ptr )
    {
      std::ifstream input_file;

      bool segment_success = file_sys_interface_.open(
        job_ptr->descriptor().getDestination().path,
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
                     job_ptr->descriptor().getDestination().path.string().c_str());
        } else liber::log::status("SegmenterThread: Finished segmenting %s\n", job_ptr->descriptor().getDestination().path.string().c_str());

        input_file.close();
      }
      else
      {
        log::debug("SegmenterThread: Failed to open %s\n.",
                     job_ptr->descriptor().getDestination().path.string().c_str());
      }

      job_ptr->signalSegmentationDone();
    }
  }
}

