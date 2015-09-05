#ifndef RSYNC_ERROR_H
#define RSYNC_ERROR_H

namespace liber {
namespace rsync {

enum RsyncError {
   kRsyncSuccess = 0,

   kRsyncJobTableInsertError,
   kRsyncAllocationError,
   kRsyncBadDescriptor,
   RsyncBadRemoteJob,

   // Authority timed out waiting for segments from the destination file.
   RsyncDestSegmentTimeout,

   RsyncDestinationFileNotFound,
   RsyncSourceFileNotFound,

   kRsyncDestinationNotWritable,
   kRsyncSourceNotReadable,
   kRsyncUnauthorizedRequest,

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
      case kRsyncSuccess: error_message = "Success"; break;
      case RsyncDestSegmentTimeout: error_message = "RsyncDestSegmentTimeout"; break;
      case RsyncDestinationFileNotFound: error_message = "Destination file not found"; break;
      case RsyncSourceFileNotFound: error_message = "Source file not found"; break;
      case RsyncRemoteQueryTimeout: error_message = "Remote job query timeout"; break;
      case kRsyncRemoteQueryError: error_message = "Remote query error"; break;
      case RsyncRemoteJobTimeout:   error_message = "Remote job timeout"; break;
      case kRsyncCommError: error_message = "Communication error"; break;
      case kRsyncNotSubscribed: error_message = "RsyncNode is not subscribed with a packet router"; break;
      case kRsyncInvalidJob: error_message = "Invalid job"; break;
      case kRsyncJobCanceled: error_message = "job canceled"; break;
      case kRsyncInvalidInstruction: error_message = "invalid assembly instruction"; break;
      case kRsyncSegmentAccessError: error_message = "segment asscess error"; break;
      case kRsyncIoError: error_message = "IO error"; break;
      case kRsyncAssemblerInvalidChunkSize: error_message = "invalid assembly chunk size"; break;
      case kRsyncNoWorkersAvailable: error_message = "no worker available"; break;
      default: break;
   }

   return error_message;
};

}
}

#endif // RSYNC_ERROR_H
