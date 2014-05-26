#ifndef RSYNC_SEGMENTER_THREAD_H
#define RSYNC_SEGMENTER_THREAD_H

#include "IThread.h"
#include "Queue.h"
#include "Segmenter.h"
#include "FileSystemInterface.h"

namespace liber {
namespace rsync {

class RsyncJob;

class SegmenterThread : public liber::concurrency::IThread {
public:

  explicit SegmenterThread(FileSystemInterface&);
  ~SegmenterThread();

  void addJob(RsyncJob* pJob);

private:

  void run(const bool& bShutdown);

private:

  Queue<RsyncJob*> mJobQueue;

  FileSystemInterface& mrFileSys;

  Segmenter mSegmenter;
};

} // End namespace rsync
} // End namespace liber

#endif // RSYNC_SEGMENTER_THREAD_H
