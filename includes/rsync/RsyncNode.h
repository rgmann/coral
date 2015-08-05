#ifndef RSYNC_CLIENT_H
#define RSYNC_CLIENT_H

// #include <boost/thread/mutex.hpp>
#include "IThread.h"
#include "Queue.h"
#include "JobDescriptor.h"
#include "JobReport.h"
#include "JobAgent.h"
#include "FileSystemInterface.h"
#include "SegmenterThread.h"
#include "AuthorityThread.h"
#include "AssemblerThread.h"
#include "RemoteAuthorityService.h"
#include "RsyncPacketRouter.h"
#include "WorkerGroup.h"
#include "RsyncJobCallback.h"

namespace liber {
namespace rsync {

class RsyncJob;

// Active Object pattern
// - Packets are routed via the RsyncNode to Authorities
// - Requesters either have the same Router as the server or
//   they are registered with a different router based on which
//   foreign node they are syncing with.
class RsyncNode {
public:

  RsyncNode( const boost::filesystem::path& root, WorkerGroup& worker_group );
  ~RsyncNode();

  void setCallback( RsyncJobCallback* callback_ptr );
  void unsetCallback();

  RsyncError sync(const boost::filesystem::path& destination,
                  const boost::filesystem::path& source,
                  bool remote_destination = false,
                  bool remote_source = false);

  RsyncError push( const boost::filesystem::path& filepath );

  RsyncError pull( const boost::filesystem::path& filepath );

  liber::netapp::PacketSubscriber& subscriber();


private:

  RsyncPacketRouter router_;

  FileSystemInterface file_sys_interface_;
  WorkerGroup& worker_group_;
  SegmenterThread segmenter_;
  AuthorityThread authority_;
  AssemblerThread assembler_;
  JobAgent        job_agent_;

  RemoteAuthorityService authority_service_;

  ui32 segment_size_;
};

} // End namespace rsync
} // End namesapce liber

#endif // RSYNC_CLIENT_H

