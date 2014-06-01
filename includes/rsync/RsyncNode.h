#ifndef RSYNC_CLIENT_H
#define RSYNC_CLIENT_H

#include "IThread.h"
#include "Queue.h"
#include "JobDescriptor.h"
#include "JobReport.h"
#include "FileSystemInterface.h"
#include "SegmenterThread.h"
#include "AuthorityThread.h"
#include "AssemblerThread.h"

namespace liber {
namespace rsync {

class RsyncJob;
//class JobDescriptor;
//class JobReport;

class RsyncJobCallback {
public:

  RsyncJobCallback() {};
  virtual ~RsyncJobCallback() {};

  virtual void call(const JobDescriptor& job, const JobReport& report) = 0;

};


// Active Object pattern
// - Packets are routed via the RsyncNode to Authorities
// - Requesters either have the same Router as the server or
//   they are registered with a different router based on which
//   foreign node they are syncing with.
class RsyncNode : public liber::concurrency::IThread {
public:

  //RsyncNode(netapp::PacketRouter& rRouter);
  RsyncNode(const boost::filesystem::path& root);
  ~RsyncNode();

  void setCompletionCallback(RsyncJobCallback* pCallback);

  bool sync(const boost::filesystem::path& destination,
            const boost::filesystem::path& source,
            bool bRemoteSource = false);

private:

  void run(const bool& bShutdown);

private:

  Mutex mCallbackLock;
  RsyncJobCallback* mpCallback;

  Queue<RsyncJob*> mPendingJobs;

  FileSystemInterface mFileSys;
  SegmenterThread mSegmenter;
  AuthorityThread mAuthority;
  AssemblerThread mAssembler;

  ui32 mnSegmentSize;
};

} // End namespace rsync
} // End namesapce liber

#endif // RSYNC_CLIENT_H

