#ifndef RSYNC_ASSEMBLER_THREAD_H
#define RSYNC_ASSEMBLER_THREAD_H

#include "IThread.h"
#include "Queue.h"
#include "SegmentFile.h"
#include "Assembler.h"

namespace liber {
namespace rsync {

class RsyncJob;
class FileSystemInterface;

class AssemblerThread : public liber::concurrency::IThread {
public:

  AssemblerThread( FileSystemInterface& file_sys_interface );
  ~AssemblerThread();

  void addJob( RsyncJob* job_ptr );

private:

  AssemblerThread( const AssemblerThread& );
  AssemblerThread& operator= ( const AssemblerThread& );

  void run(const bool& bShutdown);

private:

  Queue<RsyncJob*> job_queue_;

  class FileSystemInterface& file_sys_interface_;
  SegmentFile          segment_file_;
  Assembler            assembler_;

};

} // End namespace rsync
} // End namespace liber

#endif // RSYNC_ASSEMBLER_THREAD_H
