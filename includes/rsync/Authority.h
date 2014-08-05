#ifndef RSYNC_AUTHORITY_H
#define RSYNC_AUTHORITY_H

#include <fstream>
#include <sstream>
#include "HashTable.h"
#include "JobReport.h"
#include "SegmentHook.h"
#include "Segmenter.h"
#include "CircularBuffer.h"
//#include "NullInstructionReceiver.h"

namespace liber {
namespace rsync {

class Instruction;
class InstructionReceiver;
class JobDescriptor;

class Authority : public SegmentHook {
public:

//  Authority(JobDescriptor&,
//            liber::HashTable<16, Segment*>&,
//            InstructionReceiver&);
  Authority();
  ~Authority();

  /**
   * Takes pre-populated Segment Hash and a local file stream.  The file stream
   * is segmented using the same block size as that used to generate the Segments
   * in the Segment Hash.  Generates a set for instructions for updating the hashed
   * file to match the authoritative file.
   */
  bool process(JobDescriptor& rDescriptor, std::istream& rInStream,
               InstructionReceiver& rReceiver, SourceReport& rReport);

  liber::HashTable<16, Segment*>& hash();

private:

  void reset();

  void call(Segment& rSegment);

  enum { FlushAll = -1 };
  void flushChunkBuffer(int nFlushCount = FlushAll);

private:

//  JobDescriptor& mrDescriptor;
  liber::HashTable<16, Segment*>  mHash;

  Segmenter mSegmenter;

  ui32 mnSegmentSkipCount;
  CircularBuffer mChunkBuffer;

  ui32 mnMaxChunkSize;
  ui32 mnBufferedCount;
  Segment::ID mBufferStartID;

  ui32 mnTotalSegmentBytes;
  ui32 mnSegmentBytes;
  ui32 mnChunkBytes;

  AuthorityReport* mpReport;

//  NullInstructionReceiver mNullReceiver;
  InstructionReceiver*  mpReceiver;
};

} // End namespace rsync
} // End namespace liber

#endif // RSYNC_AUTHORITY_H
