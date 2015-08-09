#ifndef RSYNC_AUTHORITY_THREAD_H
#define RSYNC_AUTHORITY_THREAD_H

#include "IThread.h"
// #include "Queue.h"
#include "WorkerThreadTypes.h"
// #include "FileSystemInterface.h"
#include "RemoteAuthorityInterface.h"
#include "LocalAuthorityInterface.h"
#include "PacketRouter.h"

namespace liber {
namespace rsync {

// Forward declarations
class RsyncJob;

class AuthorityThread : public liber::concurrency::IThread {
public:

  AuthorityThread( JobAgentPairQueue& queue );
  ~AuthorityThread();

  // void addJob( RsyncJob* job_ptr );

  liber::netapp::PacketSubscriber& getSubscriber();
  // void setRequestID( int request_id );

private:

  void run(const bool& bShutdown);


private:

  // Queue<RsyncJob*> job_queue_;
  JobAgentPairQueue& job_queue_;

  RemoteAuthorityInterface remote_authority_;
  LocalAuthorityInterface  local_authority_;
};

} // End namespace rsync
} // End namespace liber

#endif // RSYNC_AUTHORITY_THREAD_H
