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

  JobAgent(FileSystemInterface& rFileSys,
           SegmenterThread& rSegmenter,
           AuthorityThread& rAuthority,
           AssemblerThread& rAssembler);
  ~JobAgent();

  RsyncError createJob(const ResourcePath& destination, const ResourcePath& source, ui32 nSegmentSize);

  void releaseJob(RsyncJob* pJob);

  RsyncJob* nextJob();

  bool put(const char* pData, ui32 nSizeBytes);

private:

  bool validate(const JobDescriptor& rDescriptor);

  RsyncError createJob(RsyncJob* pJob);
  RsyncError createJob(const char* pData, ui32 nSizeBytes);

  RsyncError sendRemoteJob(const JobDescriptor& descriptor);

  RsyncError error(RsyncJob* pJob, RsyncError error);

  void finishRemoteJob(const char* pData, ui32 nSizeBytes);

  RsyncError addActiveJob(RsyncJob* pJob);
  void removeActiveJob(RsyncJob* pJob);

private:

  std::map<boost::uuids::uuid, RsyncJob*> mActiveJobs;
  Queue<RsyncJob*> mReadyJobs;

  FileSystemInterface& mrFileSys;
  SegmenterThread& mrSegmenter;
  AuthorityThread& mrAuthority;
  AssemblerThread& mrAssembler;

  bool mbCreateDestStub;
};

} // End namespace rsync
} // End namespace liber

#endif // RSYNC_JOB_AGENT_H
