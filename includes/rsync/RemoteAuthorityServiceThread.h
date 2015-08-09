#ifndef RSYNC_AUTHORITY_SERVICE_THREAD_H
#define RSYNC_AUTHORITY_SERVICE_THREAD_H

#include "IThread.h"
// #include "Queue.h"
#include "WorkerThreadTypes.h"
#include "RemoteAuthorityService.h"

namespace liber {
namespace rsync {

class RsyncJob;

class RemoteAuthorityServiceThread : public liber::concurrency::IThread {
public:

  RemoteAuthorityServiceThread( JobAgentPairQueue& queue );
  ~RemoteAuthorityServiceThread();

private:

  RemoteAuthorityServiceThread( const RemoteAuthorityServiceThread& );

  RemoteAuthorityServiceThread& operator= ( const RemoteAuthorityServiceThread& );

  void run(const bool& bShutdown);


private:

  // Queue<RsyncJob*> job_queue_;
  JobAgentPairQueue& job_queue_;

  RemoteAuthorityService authority_;
};

} // End namespace rsync
} // End namespace liber

#endif // RSYNC_AUTHORITY_SERVICE_THREAD_H
