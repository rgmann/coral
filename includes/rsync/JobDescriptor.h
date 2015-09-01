#ifndef RSYNC_JOB_DESCRIPTOR_H
#define RSYNC_JOB_DESCRIPTOR_H

#include <string>
#include <boost/filesystem.hpp>
#include <boost/uuid/uuid.hpp>
#include "BaseTypes.h"
#include "Serializable.h"
#include "ResourcePath.h"


namespace liber {
namespace rsync {

struct JobLimits {
   ui32 segment_size_bytes;
   ui32 maximum_chunk_size_bytes;
   ui32 completion_timeout_ms;

   JobLimits()
   : segment_size_bytes( 256 )
   , maximum_chunk_size_bytes( 512 )
   , completion_timeout_ms( 10000 )
   {}
};

class JobDescriptor : public liber::netapp::Serializable {
public:

   JobDescriptor();

   void  setRemoteRequest();
   bool  isRemoteRequest() const;

   void  setSegmentSize(ui32 nSegmentSizeBytes);
   ui32  getSegmentSize() const;

   void  setLimits( const JobLimits& limits );

   ui32  completionTimeoutMs() const;

   void  setSource(const boost::filesystem::path&, bool bRemote = false);
   void  setSource(const ResourcePath& path);
   const boost::filesystem::path& getSourcePath() const;
   const ResourcePath& getSource() const;
   ResourcePath& getSource();

   void  setDestination(const boost::filesystem::path&, bool bRemote = false);
   void  setDestination(const ResourcePath& path);
   const boost::filesystem::path& getDestinationPath() const;
   const ResourcePath& getDestination() const;
   ResourcePath& getDestination();

   bool  isValid() const;

   const boost::uuids::uuid& uuid() const;

   ui32 getMaximumChunkSize() const;


protected:

   void  pack( liber::netapp::SerialStream& ctor );
   void  pack( liber::netapp::SerialStream& ctor ) const;
   bool  unpack( liber::netapp::SerialStream& dtor );

private:

   JobLimits limits_;

   ResourcePath source_path_;
   ResourcePath destination_path_;

   bool remotely_requested_;

   boost::uuids::uuid uuid_;
};

} // End namespace rsync
} // End namespace liber

#endif // RSYNC_JOB_DESCRIPTOR_H
