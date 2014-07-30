#ifndef RSYNC_ERROR_H
#define RSYNC_ERROR_H

namespace liber {
namespace rsync {

enum RsyncError {
  RsyncSuccess = 0,

  RsyncJobTableInsertionError,
  RsyncQueueError,
  RsyncBadDescriptor,
  RsyncBadRemoteJob,

  // Authority timed out waiting for segments from the destination file.
  RsyncDestSegmentTimeout,

  RsyncDestinationFileNotFound,
  RsyncSourceFileNotFound,

  RsyncRemoteQueryTimeout,
  RsyncRemoteJobTimeout
};

inline std::string errorToString(RsyncError error)
{
  switch (error)
  {
    case RsyncSuccess: return std::string("Success");
    case RsyncDestSegmentTimeout: return std::string("RsyncDestSegmentTimeout");
    case RsyncDestinationFileNotFound: return std::string("Destination file not found");
    case RsyncSourceFileNotFound: return std::string("Source file not found");
    case RsyncRemoteQueryTimeout: return std::string("Remote job query timeout");
    case RsyncRemoteJobTimeout: return std::string("Remote job timeout");
    default: return std::string("Unknown error code");
  }
};

}
}

#endif // RSYNC_ERROR_H
