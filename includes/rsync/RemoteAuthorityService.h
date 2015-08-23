#ifndef RSYNC_REMOTE_AUTHORITY_SERVICE_H
#define RSYNC_REMOTE_AUTHORITY_SERVICE_H

#include <boost/thread/mutex.hpp>
#include "IThread.h"
#include "Queue.h"
#include "RsyncError.h"
#include "PacketSubscriber.h"
#include "FileSystemInterface.h"
#include "LocalAuthorityInterface.h"
#include "InstructionHook.h"

namespace liber {
namespace rsync {

class JobDescriptor;

class RemoteAuthorityService
: public liber::netapp::PacketSubscriber
, public InstructionHook {
public:

  RemoteAuthorityService();
  ~RemoteAuthorityService();

  void setRequestID(int nRequestID);

  bool put( liber::netapp::DestinationID destination_id, const void* pData, ui32 nLength);

  typedef RsyncError (*QueryHandler)(JobDescriptor&);
  void setQueryHandler(QueryHandler pFunc);

  void process( RsyncJob* job_ptr );

private:

  void call(Instruction* pInstruction);

  void handleRemoteJobRequest(const void* pData, ui32 nLength);

  RsyncError defaultQueryHandler(JobDescriptor& rJob);

  void pushSegment(const void* pData, ui32 nLength);

private:

  boost::mutex active_job_lock_;
  RsyncJob* active_job_;

  LocalAuthorityInterface authority_;

  QueryHandler mpUserHandler;

  int mRequestID;

};

} // End namespace rsync
} // End namespace liber

#endif // RSYNC_REMOTE_AUTHORITY_SERVICE_H
