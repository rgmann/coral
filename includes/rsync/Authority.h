#ifndef RSYNC_AUTHORITY_H
#define RSYNC_AUTHORITY_H

#include <fstream>
#include <sstream>
#include "HashTable.h"
#include "JobReport.h"
#include "SegmentHook.h"
#include "Segmenter.h"

namespace liber {
namespace rsync {

class Instruction;
class JobDescriptor;
class InstructionReceiver;

class Authority : public SegmentHook {
public:

  Authority(JobDescriptor&,
            liber::HashTable<16, Segment*>&,
            InstructionReceiver&);
  ~Authority();

  /**
   * Takes pre-populated Segment Hash and a local file stream.  The file stream
   * is segmented using the same block size as that used to generate the Segments
   * in the Segment Hash.  Generates a set for instructions for updating the hashed
   * file to match the authoritative file.
   */
  bool process(std::istream& rInStream,
               JobReport::SourceReport& rReport);

private:

  void call(Segment& rSegment);

  //Instruction* createInstruction(std::stringstream& stream);
  Instruction* createInstruction(std::string& stream);

  void flushChunkBuffer();

private:

  JobDescriptor& mrDescriptor;
  liber::HashTable<16, Segment*>&  mrHash;
  InstructionReceiver&  mrReceiver;

  Segmenter mSegmenter;

  ui32 mnSegmentSkipCount;
  //std::stringstream mChunkBuffer;
  std::string mChunkBuffer;

  ui32 mnMaxChunkSize;
  ui32 mnBufferedCount;

  AuthorityReport* mpReport;
};

} // End namespace rsync
} // End namespace liber

#endif // RSYNC_AUTHORITY_H
