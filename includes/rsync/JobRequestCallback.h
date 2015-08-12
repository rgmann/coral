#ifndef  JOB_REQUEST_CALLBACK_H
#define  JOB_REQUEST_CALLBACK_H

#include <string>
#include "RsyncError.h"

namespace liber {
namespace rsync {

class JobDescriptor;
class FileSystemInterface;

class JobRequestCallback {
public:

   JobRequestCallback( bool create_permitted = true );

   virtual ~JobRequestCallback(){};

   bool canTouchDestination() const;

   RsyncError canFulfill( const FileSystemInterface& file_sys, const JobDescriptor& descriptor );

protected:

   virtual bool authorized( const JobDescriptor& descriptor ) { return true; }

   virtual bool resourceReadable(
      const FileSystemInterface& file_sys,
      const JobDescriptor& descriptor,
      std::string& message );

   virtual bool resourceWritable(
      const FileSystemInterface& file_sys,
      const JobDescriptor& descriptor,
      std::string& message );

private:

   bool create_permitted_;
};

}
}

#endif // JOB_REQUEST_CALLBACK_H