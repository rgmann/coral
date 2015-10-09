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



#ifndef RSYNC_SEGMENT_FILE_H
#define RSYNC_SEGMENT_FILE_H

#include <fstream>
#include "Segmenter.h"
#include "SegmentAccessor.h"

namespace liber {
namespace rsync {

class JobDescriptor;
class FileSystemInterface;

// Since SegmentIDs are simply sequential indeces of blocks, it is simple
// enough to open the file and seek forward into the file if the chunk size
// is known.
class SegmentFile : public SegmentAccessor {
public:

  SegmentFile();
  ~SegmentFile();

  bool open( FileSystemInterface& file_sys_interface, const JobDescriptor& descriptor );
  void close();

  Segment* getSegment(Segment::ID id);

private:

  const JobDescriptor* mpDescriptor;
  std::ifstream mIStream;

  // FileSystemInterface& mrFileSys;
};

} // End namespace rsync
} // End namespace liber


#endif // RSYNC_SEGMENT_FILE_H
