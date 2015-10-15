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
#include "FileSystemInterface.h"
#include "JobDescriptor.h"
#include "SegmentFile.h"

using namespace coral;
using namespace coral::rsync;

//-----------------------------------------------------------------------------
SegmentFile::SegmentFile()
: SegmentAccessor()
, mpDescriptor(NULL)
{
}

//-----------------------------------------------------------------------------
SegmentFile::~SegmentFile()
{
  close();
}

//-----------------------------------------------------------------------------
bool SegmentFile::open(
  FileSystemInterface&               file_sys_interface,
  const coral::rsync::JobDescriptor& descriptor)
{
  close();
  mpDescriptor = &descriptor;
  return file_sys_interface.open( descriptor.getDestination().path(), mIStream );
}

//-----------------------------------------------------------------------------
void SegmentFile::close()
{
  mIStream.close();
}

//-----------------------------------------------------------------------------
Segment* SegmentFile::getSegment(coral::rsync::Segment::ID id)
{
  Segment* lpSegment = NULL;

  if (mpDescriptor)
  {
    if ((id >= 0) && mIStream.is_open())
    {
      mIStream.seekg(id * mpDescriptor->getSegmentSize(), mIStream.beg);
      lpSegment = new Segment(id, id * mpDescriptor->getSegmentSize());
      lpSegment->setData(mIStream, mpDescriptor->getSegmentSize());
    }
  }

  return lpSegment;
}


