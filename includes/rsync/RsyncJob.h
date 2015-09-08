#ifndef RSYNC_JOB_H
#define RSYNC_JOB_H

#include <boost/filesystem.hpp>
#include "BinarySem.h"
#include "JobDescriptor.h"
#include "JobReport.h"
#include "SegmentQueue.h"
#include "InstructionQueue.h"

namespace liber {
namespace rsync {

class FileSystemInterface;
class RsyncPacketRouter;
class RsyncJobCallback;

class RsyncJob {
public:

  RsyncJob( FileSystemInterface& file_sys_interface, RsyncPacketRouter& router );
  ~RsyncJob();

  void setDescriptor( const JobDescriptor& descriptor );
  JobDescriptor& descriptor();

  JobReport& report();
  void mergeReport(const JobReport& rReport);

  SegmentQueue& segments();

  InstructionQueue& instructions();

  void signalSegmentationDone();
  void signalAuthDone();
  void signalAssemblyDone();
  void signalAllDone();

  bool done() const;
  bool waitDone(int nTimeoutMs = liber::thread::Semaphore::SemWaitForever);

  RsyncPacketRouter& packetRouter(){ return router_; };

  FileSystemInterface& fileSystem(){ return file_sys_interface_; };

private:
 
  RsyncJob(const RsyncJob&);
  RsyncJob& operator= (const RsyncJob&);

private:

  JobDescriptor mDescriptor;

  RsyncPacketRouter& router_;
  FileSystemInterface& file_sys_interface_;

  JobReport report_;

  SegmentQueue mSegments;
  InstructionQueue mInstructions;

  BinarySem mSegmentationDone;
  BinarySem mAuthDone;
  BinarySem mAssemblyDone;
};

} // End namespace rsync
} // End namespace liber

#endif // RSYNC_JOB_H
