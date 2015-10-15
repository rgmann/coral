// 
// Copyright (c) 2015, Robert Glissmann
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// 
// * Redistributions of source code must retain the above copyright notice, this
// list of conditions and the following disclaimer.
// 
// * Redistributions in binary form must reproduce the above copyright notice,
// this list of conditions and the following disclaimer in the documentation
// and/or other materials provided with the distribution.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// 

// %% license-end-token %%
// 
// Author: Robert.Glissmann@gmail.com (Robert Glissmann)
// 
// 



#ifndef RSYNC_ERROR_H
#define RSYNC_ERROR_H

namespace coral {
namespace rsync {

enum RsyncError {
   kRsyncSuccess = 0,

   kRsyncJobTableInsertError,
   kRsyncAllocationError,
   kRsyncBadDescriptor,
   kRsyncBadRemoteJob,

   // Authority timed out waiting for segments from the destination file.
   kRsyncDestSegmentTimeout,

   kRsyncDestinationFileNotFound,
   kRsyncSourceFileNotFound,

   kRsyncDestinationNotWritable,
   kRsyncSourceNotReadable,
   kRsyncUnauthorizedRequest,

   kRsyncRemoteQueryTimeout,
   kRsyncRemoteQueryError,
   kRsyncRemoteJobTimeout,

   kRsyncCommError,
   kRsyncNotSubscribed,

   kRsyncInvalidJob,
   kRsyncJobCanceled,
   kRsyncInvalidInstruction,
   kRsyncSegmentAccessError,
   kRsyncIoError,
   kRsyncMemAllocationFailed,

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
      case kRsyncDestSegmentTimeout: error_message = "kRsyncDestSegmentTimeout"; break;
      case kRsyncDestinationFileNotFound: error_message = "Destination file not found"; break;
      case kRsyncSourceFileNotFound: error_message = "Source file not found"; break;
      case kRsyncRemoteQueryTimeout: error_message = "Remote job query timeout"; break;
      case kRsyncRemoteQueryError: error_message = "Remote query error"; break;
      case kRsyncRemoteJobTimeout:   error_message = "Remote job timeout"; break;
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
