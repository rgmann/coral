#ifndef RSYNC_JOB_AGENT_H
#define RSYNC_JOB_AGENT_H

#include <map>
#include <boost/uuid/uuid.hpp>
#include <boost/thread/mutex.hpp>
#include "PacketSubscriber.h"

namespace liber {
namespace rsync {

class RsyncJob;
class FileSystemInterface;
class RsyncPacketRouter;
class RsyncJobCallback;
class WorkerGroup;

class JobAgent : public liber::netapp::PacketSubscriber {
public:

   JobAgent( FileSystemInterface& file_sys_interface,
            RsyncPacketRouter&   packet_router,
            WorkerGroup&         worker_group );

   ~JobAgent();

   RsyncError createJob( const ResourcePath& destination, const ResourcePath& source );

   void releaseJob( RsyncJob* job_ptr );

   RsyncJob* nextJob();

   void setCallback( RsyncJobCallback* callback_ptr );
   void unsetCallback();

   bool put( const char* data_ptr, ui32 size_bytes );

   JobLimits limits_;

private:

   bool validate( const JobDescriptor& descriptor );

   void handleRemoteJobRequest( const void* pData, ui32 nLength );

   RsyncError createJob( RsyncJob* job_ptr );
   RsyncError createJob( const char* data_ptr, ui32 size_bytes );

   RsyncError sendRemoteJob( const JobDescriptor& descriptor );

   RsyncError error( RsyncJob* job_ptr, RsyncError error );

   void finishRemoteJob(const char* data_ptr, ui32 size_bytes );

   RsyncError addActiveJob( RsyncJob* job_ptr );
   void removeActiveJob( RsyncJob* job_ptr );

private:

   std::map<boost::uuids::uuid, RsyncJob*> active_jobs_;
   Queue<RsyncJob*> ready_jobs_;

   FileSystemInterface& file_sys_interface_;
   RsyncPacketRouter&   packet_router_;
   WorkerGroup&         worker_group_;

   bool create_destination_stub_;

   boost::mutex callback_lock_;
   RsyncJobCallback* callback_ptr_;
};

} // End namespace rsync
} // End namespace liber

#endif // RSYNC_JOB_AGENT_H
