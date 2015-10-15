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



#ifndef RSYNC_SEGMENT_H
#define RSYNC_SEGMENT_H

#include <fstream>
#include <istream>
#include "BaseTypes.h"
#include "Md5Hash.h"
#include "Serializable.h"


namespace coral {

class CircularBuffer;
  
namespace rsync {


struct Adler32Checksum {
  i32 s;
  i32 a;
  i32 b;

  ui32   k;
  ui8  x_k;

  ui32   l;
  ui8  x_l;

  i32 checksum() const
  {
    return s;
  }
};


class Segment : public coral::netapp::Serializable {
public:

  typedef i32 ID;
  enum { EndOfStream = -1 };

  Segment();
  Segment(ID id, ui32 offset);
  Segment(ui32 nVSegmentSize);
  Segment(const Segment&);
  ~Segment();

  inline void setID(ID id) { segment_id_ = id; };
  inline void setOffset(ui32 offset) { offset_ = offset; };

  void setData(const ui8* pData, ui32 nVSegmentSize, ui32 nSegmentSize, const Adler32Checksum* pPrev = NULL);
  void setData(CircularBuffer& buffer, ui32 nVSegmentSize, const Adler32Checksum* pPrev = NULL);
  void setData(std::istream& stream, ui32 nVSegmentSize, const Adler32Checksum* pPrev = NULL);

  const Adler32Checksum& getWeak();

  Md5Hash& getStrong();
  Md5Hash& getStrong(std::ifstream&);

  ui8* const data();
  ui8* const data(std::ifstream&);
  ui32 size() const;

  std::string debugDump();

  ID getID() const;

  /**
   * The end of a Segment stream is signalled with Segment is with an invalid
   * ID of -1.
   */
  bool endOfStream() const;

  /**
   * 
   */
  void release();

  enum OffsetBase {
    StreamStart,
    SegmentStart
  };
  ui8 getByte(ui32 nOffset, OffsetBase base = SegmentStart) const;

  bool isValid() const;

protected:

  bool resize( ui32 virtual_segment_size );

  void pack(coral::netapp::SerialStream&) const;
  void pack(coral::netapp::SerialStream&);
  bool unpack(coral::netapp::SerialStream& dtor);

private:

  void computeWeak( const Adler32Checksum* previous_weak_checksum_ptr );

  void computeWeak(Adler32Checksum &next, 
                          ui32 nOffset,
                          ui32 nBlockSizeBytes);

  void rollWeak(Adler32Checksum &next,
                       ui32 nOffset,
                       ui32 nBlockSizeBytes,
                       const Adler32Checksum &prev);


private:

  ID segment_id_;

  // Small, inexpensive-to-compute, rolling checksum for first-lay segment
  // matching.
  Adler32Checksum weak_checksum_;

  // 128-bit strong checksum is much more expensive to compute, but will
  // encounter virtually zero collisions. This checksum is used for second-
  // layer segment matching.
  Md5Hash strong_checksum_;

  // Pointer to segment data.
  ui8* data_ptr_;

  // Actual segment size in bytes. This may be less than the number of bytes
  // allocated (see virtual_segment_size_).
  ui32 segment_size_;

  // Offset of this segment within the file, in bytes.
  ui32 offset_; 

  // All segments are the same size, except at the end of a file.  If the
  // segment size is large and/or segmentation is being performed at every
  // offset, this can result in many segments with different sizes. To
  // eliminate the need for re-allocation on each, each Segment has an
  // actual size and an allocated/virtual size.
  ui32 virtual_segment_size_;
};


class SegmentComparator {
public:

  SegmentComparator( Segment* segment_ptr );

  bool operator() ( Segment* segment_ptr );

private:

  Segment* segment_ptr_;
};


class SegmentDestructor {
public:
  void operator() ( Segment*& segment_ptr );
};


} // End of namespace rsync
} // End of namespace coral


#endif // RSYNC_SEGMENT_H
