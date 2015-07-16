#ifndef RSYNC_JOB_AGENT_H
#define RSYNC_JOB_AGENT_H

#include <map>
#include <boost/uuid/uuid.hpp>
#include "PacketSubscriber.h"

namespace liber {
namespace rsync {

class RsyncJob;
class FileSystemInterface;
class SegmenterThread;
class AuthorityThread;
class AssemblerThread;

class JobAgent : public liber::netapp::PacketSubscriber {
public:

  JobAgent( FileSystemInterface& file_sys_interface,
            SegmenterThread&     segmenter,
            AuthorityThread&     authority,
            AssemblerThread&     assembler );
  ~JobAgent();

  RsyncError createJob(
    const ResourcePath& destination,
    const ResourcePath& source,
    ui32 segment_size );

  void releaseJob( RsyncJob* job_ptr );

  RsyncJob* nextJob();

  bool put( const char* data_ptr, ui32 size_bytes );

private:

  bool validate( const JobDescriptor& descriptor );

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
  SegmenterThread& segmenter_;
  AuthorityThread& authority_;
  AssemblerThread& assembler_;

  bool create_destination_stub_;
};

} // End namespace rsync
} // End namespace liber

#endif // RSYNC_JOB_AGENT_H
