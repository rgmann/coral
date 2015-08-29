#ifndef RSYNC_NODE_H
#define RSYNC_NODE_H

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

   void setJobCompletionCallback( RsyncJobCallback* callback_ptr );
   void unsetJobCompletionCallback();

   void setJobRequestCallback( JobRequestCallback* callback_ptr );
   void unsetJobRequestCallback();

   bool setSegmentSize( ui32 segment_size_bytes );
   bool setMaximumChunkSize( ui32 maximum_chunk_size_bytes );
   bool setCompletionTimeout( ui32 completion_timeout_ms );

   //
   // Synchronize two files.
   //
   //
   RsyncError sync( const ResourcePath& destination, const ResourcePath& source );

   RsyncError push( const boost::filesystem::path& filepath );

   RsyncError pull( const boost::filesystem::path& filepath );

   liber::netapp::PacketSubscriber& subscriber();


private:

   RsyncPacketRouter router_;

   FileSystemInterface file_sys_interface_;
   // WorkerGroup& worker_group_;
   JobAgent        job_agent_;
};

} // End namespace rsync
} // End namesapce liber

#endif // RSYNC_NODE_H

