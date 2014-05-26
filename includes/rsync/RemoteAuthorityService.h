#ifndef RSYNC_REMOTE_AUTHORITY_SERVICE_H
#define RSYNC_REMOTE_AUTHORITY_SERVICE_H

#include "IThread.h"
#include "Queue.h"
#include "RsyncError.h"
#include "PacketSubscriber.h"
#include "AuthorityInterface.h"
#include "FileSystemInterface.h"
#include "LocalAuthorityInterface.h"

namespace liber {
namespace rsync {

class JobDescriptor;

class RemoteAuthorityService
: public liber::netapp::PacketSubscriber
, public liber::concurrency::IThread {
public:

  RemoteAuthorityService(FileSystemInterface&);
  ~RemoteAuthorityService();

  bool put(const char* pData, ui32 nLength);

  typedef RsyncError (*QueryHandler)(JobDescriptor&);
  void setQueryHandler(QueryHandler pFunc);

private:

  void run(const bool& bShutdown);

  void handleRemoteJobRequest(const void* pData, ui32 nLength);

  RsyncError defaultQueryHandler(JobDescriptor& rJob);

  void pushSegment(const void* pData, ui32 nLength);

private:

  Queue<RsyncJob*> mJobQueue;

  FileSystemInterface& mrFileSys;

  LocalAuthorityInterface mAuthorityInterface;

  QueryHandler mpUserHandler;
};

} // End namespace rsync
} // End namespace liber

#endif // RSYNC_REMOTE_AUTHORITY_SERVICE_H
