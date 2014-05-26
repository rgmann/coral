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

class RsyncJob {
public:

  RsyncJob();
  ~RsyncJob();

  JobDescriptor& descriptor();

  JobReport& report();

  SegmentQueue& segments();

  InstructionQueue& instructions();

  void signalDone();
  bool waitDone(int nTimeoutMs = Sem::SemWaitForever);

private: 
  RsyncJob(const RsyncJob&);
  RsyncJob& operator= (const RsyncJob&);

private:

  JobDescriptor mDescriptor;

  JobReport mReport;

  SegmentQueue mSegments;
  InstructionQueue mInstructions;

  BinarySem mDoneCondition;
};

} // End namespace rsync
} // End namespace liber

#endif // RSYNC_JOB_H

