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
         bool resource_exists = file_sys.exists(
            descriptor.getSourcePath() );

         if ( resource_exists == false )
         {
            status = RsyncSourceFileNotFound;
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
            status = RsyncDestinationFileNotFound;
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
