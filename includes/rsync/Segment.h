#ifndef RSYNC_SEGMENT_H
#define RSYNC_SEGMENT_H

#include <fstream>
#include <istream>
#include "BaseTypes.h"
#include "Md5Hash.h"
#include "PacketHelper.h"

class CircularBuffer;

namespace liber {
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


class Segment : public liber::netapp::Serializable {
public:

  typedef i32 ID;
  enum { EndOfStream = -1 };

  Segment();
  Segment(ID id, ui32 offset);
  Segment(ui32 nVSegmentSize);
  Segment(const Segment&);
  ~Segment();

  inline void setID(ID id) { mID = id; };
  inline void setOffset(ui32 offset) { mnOffset = offset; };

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

  void pack(liber::netapp::PacketCtor&) const;
  void pack(liber::netapp::PacketCtor&);
  bool unpack(liber::netapp::PacketDtor& dtor);

private:

  void computeWeak(Adler32Checksum &next, 
                          ui32 nOffset,
                          ui32 nBlockSizeBytes);

  void rollWeak(Adler32Checksum &next,
                       ui32 nOffset,
                       ui32 nBlockSizeBytes,
                       const Adler32Checksum &prev);


private:

  ID mID;

  Adler32Checksum mWeak;

  Md5Hash mStrong;

  ui8* mpData;
  ui32 mnSegmentSize;  // Actual segment size
  ui32 mnOffset; 

  // For efficient memory re-use, the buffer is allocated to configured
  // segment size.
  ui32 mnVSegmentSize;
};


class SegmentComparator {
public:

  SegmentComparator(Segment* pSegment);

  bool operator() (Segment* pSegment);

private:

  Segment* mpSegment;
};


class SegmentDestructor {
public:
  void operator() (Segment*& pSegment);
};


} // End of namespace rsync
} // End of namespace liber


#endif // RSYNC_SEGMENT_H
