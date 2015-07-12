#ifndef RSYNC_SEGMENTER_THREAD_H
#define RSYNC_SEGMENTER_THREAD_H

#include "IThread.h"
#include "Queue.h"
#include "FileSystemInterface.h"

namespace liber {
namespace rsync {

class RsyncJob;

class SegmenterThread : public liber::concurrency::IThread {
public:

  explicit SegmenterThread( FileSystemInterface& );
  ~SegmenterThread();

  void addJob( RsyncJob* job_ptr );

private:

  SegmenterThread( const SegmenterThread& );

  SegmenterThread& operator= ( const SegmenterThread& );

  void run(const bool& bShutdown);


private:

  Queue<RsyncJob*> job_queue_;

  FileSystemInterface& file_sys_interface_;
};

} // End namespace rsync
} // End namespace liber

#endif // RSYNC_SEGMENTER_THREAD_H
