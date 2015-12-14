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



#ifndef RSYNC_JOB_DESCRIPTOR_H
#define RSYNC_JOB_DESCRIPTOR_H

#include <string>
#include <boost/filesystem.hpp>
#include <boost/uuid/uuid.hpp>
#include "BaseTypes.h"
#include "Serializable.h"
#include "ResourcePath.h"


namespace coral {
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

class JobDescriptor : public coral::netapp::Serializable {
public:

   JobDescriptor();
   JobDescriptor( const void* data_ptr, ui32 length );

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

   // void  pack( coral::netapp::SerialStream& ctor );
   void  pack( coral::netapp::SerialStream& ctor ) const;
   bool  unpack( coral::netapp::SerialStream& dtor );

private:

   JobLimits limits_;

   ResourcePath source_path_;
   ResourcePath destination_path_;

   bool remotely_requested_;

   boost::uuids::uuid uuid_;
};

} // End namespace rsync
} // End namespace coral

#endif // RSYNC_JOB_DESCRIPTOR_H
