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



#include "Log.h"
#include "JobDescriptor.h"
#include "FileSystemInterface.h"
#include "JobRequestCallback.h"

using namespace liber;
using namespace liber::rsync;

//-----------------------------------------------------------------------------
JobRequestCallback::JobRequestCallback( bool create_permitted )
:  create_permitted_ ( create_permitted )
{
}

//-----------------------------------------------------------------------------
RsyncError JobRequestCallback::canFulfill(
   const FileSystemInterface& file_sys,
   const JobDescriptor&       descriptor
)
{
   RsyncError status = kRsyncSuccess;
   std::string error_message;

   if ( authorized( descriptor ) )
   {
      if ( descriptor.getSource().remote() == false )
      {
         bool resource_exists = file_sys.exists( descriptor.getSourcePath() );

         if ( resource_exists == false )
         {
            status = kRsyncSourceFileNotFound;
         }
         else
         {
            bool resource_readable = resourceReadable(
               file_sys, descriptor, error_message );

            if ( resource_readable == false )
            {
               status = kRsyncDestinationNotWritable;
            }
         }
      }

      if ( descriptor.getDestination().remote() == false )
      {
         bool resource_exists = file_sys.exists(
            descriptor.getDestinationPath() );

         if ( ( resource_exists == false ) && ( create_permitted_ == false ) )
         {
            status = kRsyncDestinationFileNotFound;
         }
         else
         {
            bool resource_writable = resourceWritable(
               file_sys, descriptor, error_message );

            if ( resource_writable == false )
            {
               status = kRsyncDestinationNotWritable;
            }
         }
      }
   }
   else
   {
      status = kRsyncUnauthorizedRequest;
   }

   return status;
}

//-----------------------------------------------------------------------------
bool JobRequestCallback::canTouchDestination() const
{
   return create_permitted_;
}

//-----------------------------------------------------------------------------
bool JobRequestCallback::resourceReadable(
   const FileSystemInterface& file_sys,
   const JobDescriptor& descriptor,
   std::string& message )
{
   return true;
}

//-----------------------------------------------------------------------------
bool JobRequestCallback::resourceWritable(
   const FileSystemInterface& file_sys,
   const JobDescriptor& descriptor,
   std::string& message )
{
   return true;
}
