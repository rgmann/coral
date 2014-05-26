#ifndef RSYNC_AUTHORITY_INTERFACE_H
#define RSYNC_AUTHORITY_INTERFACE_H

#include "IThread.h"
#include "Queue.h"
//#include "RsyncJob.h"

namespace liber {
namespace rsync {

class RsyncJob;

class AuthorityInterface {
public:

  AuthorityInterface();
  virtual ~AuthorityInterface();

  void process(RsyncJob* pJob);

  bool lockActiveJob();
  void unlockActiveJob();
  RsyncJob* activeJob();

protected:

  virtual void processJob(RsyncJob* pJob) = 0;

private:

  RsyncJob* mpActiveJob;
  Mutex mActiveJobLock;
};

} // End namespace rsync
} // End namespace liber

#endif // RSYNC_AUTHORITY_INTERFACE_H
