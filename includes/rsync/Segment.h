#ifndef RSYNC_SEGMENT_H
#define RSYNC_SEGMENT_H

#include <fstream>
#include "BaseTypes.h"
#include "Md5Hash.h"

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


class Segment {
public:

  typedef i32 ID;

  Segment();
//  Segment(ID id, ui32 offset, Adler32Checksum*, Md5Hash*);
  Segment(ID id, ui32 offset);
  Segment(const Segment&);
  ~Segment();

  void setData(const ui8* pData, ui32 nVSegmentSize, ui32 nSegmentSize, const Adler32Checksum* pPrev = NULL);

  const Adler32Checksum& getWeak();

  Md5Hash& getStrong();
  Md5Hash& getStrong(std::ifstream&);

  ui8* const data();
  ui8* const data(std::ifstream&);
  ui32 size() const;

  std::string debugDump();

  ID getID() const;

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
//  bool pack(ui8** ppData, ui32& nSizeBytes);
//  bool unpack(ui8* const pData, ui32 nSizeBytes);

  std::string serialize();
  bool deserialize(const std::string& data);

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
  ui32 mnSegmentSize;
  ui32 mnOffset; 

};

} // End of namespace rsync
} // End of namespace liber


#endif // RSYNC_SEGMENT_H
