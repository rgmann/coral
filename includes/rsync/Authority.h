#ifndef RSYNC_AUTHORITY_H
#define RSYNC_AUTHORITY_H

#include <fstream>
#include <sstream>
#include "SegmentHash.h"
#include "SegmentHook.h"
#include "InstructionReceiver.h"

namespace liber {
namespace rsync {

class Authority : public liber::rsync::SegmentHook {
public:

  Authority(liber::rsync::JobDescriptor&,
            liber::rsync::SegmentHash&,
            liber::rsync::InstructionReceiver&);
  ~Authority();

  /**
   * Takes pre-populated Segment Hash and a local file stream.  The file stream
   * is segmented using the same block size as that used to generate the Segments
   * in the Segment Hash.  Generates a set for instructions for updating the hashed
   * file to match the authoritative file.
   */
  bool process(std::istream& rInStream);

private:

  void call(Segment* pSegment);

  Instruction* createInstruction(std::stringstream& stream);

private:

  liber::rsync::JobDescriptor mDescriptor;
  liber::rsync::SegmentHash&  mrHash;
  liber::rsync::InstructionReceiver&  mrReceiver;

  ui32 mnSegmentSkipCount;
  std::stringstream mChunkBuffer;

  ui32 mnMaxChunkSize;
};

} // End namespace rsync
} // End namespace liber

#endif // RSYNC_AUTHORITY_H
