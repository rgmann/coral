#ifndef RSYNC_ERROR_H
#define RSYNC_ERROR_H

namespace liber {
namespace rsync {

enum RsyncError {
   RsyncSuccess = 0,

   kRsyncJobTableInsertError,
   kRsyncAllocationError,
   kRsyncBadDescriptor,
   RsyncBadRemoteJob,

   // Authority timed out waiting for segments from the destination file.
   RsyncDestSegmentTimeout,

   RsyncDestinationFileNotFound,
   RsyncSourceFileNotFound,

   RsyncRemoteQueryTimeout,
   kRsyncRemoteQueryError,
   RsyncRemoteJobTimeout,

   kRsyncCommError,
   kRsyncNotSubscribed,

   kRsyncInvalidJob,
   kRsyncJobCanceled,
   kRsyncInvalidInstruction,
   kRsyncSegmentAccessError,
   kRsyncIoError,

   kRsyncAssemblerInvalidChunkSize,
   kRsyncNoWorkersAvailable,

   kRsyncErrorCount
};

inline std::string errorToString(RsyncError error)
{
   std::string error_message( "Unknown error code" );

   switch (error)
   {
      case RsyncSuccess: error_message = "Success"; break;
      case RsyncDestSegmentTimeout: error_message = "RsyncDestSegmentTimeout"; break;
      case RsyncDestinationFileNotFound: error_message = "Destination file not found"; break;
      case RsyncSourceFileNotFound: error_message = "Source file not found"; break;
      case RsyncRemoteQueryTimeout: error_message = "Remote job query timeout"; break;
      case RsyncRemoteJobTimeout:   error_message = "Remote job timeout"; break;
      case kRsyncAssemblerInvalidChunkSize: error_message = "Chunk size is exceeded limit"; break;
      default: break;
   }

   return error_message;
};

}
}

#endif // RSYNC_ERROR_H
