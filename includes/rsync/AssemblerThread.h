#ifndef RSYNC_ASSEMBLER_THREAD_H
#define RSYNC_ASSEMBLER_THREAD_H

#include "IThread.h"
#include "Queue.h"
#include "SegmentFile.h"
#include "Assembler.h"

namespace liber {
namespace rsync {

class RsyncJob;

class AssemblerThread : public liber::concurrency::IThread {
public:

  explicit AssemblerThread(class FileSystemInterface&);
  ~AssemblerThread();

  void addJob(RsyncJob* pJob);

private:

  void run(const bool& bShutdown);

private:

  Queue<RsyncJob*> mJobQueue;

  class FileSystemInterface& mrFileSys;
  SegmentFile          mSegmentFile;
  Assembler            mAssembler;

};

} // End namespace rsync
} // End namespace liber

#endif // RSYNC_ASSEMBLER_THREAD_H
