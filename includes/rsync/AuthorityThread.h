#ifndef RSYNC_AUTHORITY_THREAD_H
#define RSYNC_AUTHORITY_THREAD_H

#include "IThread.h"
#include "Queue.h"
#include "FileSystemInterface.h"
#include "RemoteAuthorityInterface.h"
#include "LocalAuthorityInterface.h"


namespace liber {
namespace rsync {

// Forward declarations
class RsyncJob;

class AuthorityThread : public liber::concurrency::IThread {
public:

  explicit AuthorityThread(FileSystemInterface&);
  ~AuthorityThread();

  void addJob(RsyncJob* pJob);

private:

  void run(const bool& bShutdown);

private:

  Queue<RsyncJob*> mJobQueue;

  RemoteAuthorityInterface mRemoteAuthority;
  LocalAuthorityInterface  mLocalAuthority;
};

} // End namespace rsync
} // End namespace liber

#endif // RSYNC_AUTHORITY_THREAD_H
