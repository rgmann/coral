#ifndef RSYNC_ASSEMBLER_H
#define RSYNC_ASSEMBLER_H

#include <fstream>
//#include "InstructionQueue.h"
#include "SegmentAccessor.h"

namespace liber {
namespace rsync {


class Assembler {
public:

  explicit Assembler(SegmentAccessor&);
  ~Assembler();

  bool process(class InstructionQueue& rQueue, struct AssemblyReport& rReport);

  /**
   * Process the next assembly instruction.
   */
  //bool execute(Instruction* pInstruction);

  /**
   *
   */
  const ExecutionStatus& status() const;

  std::ofstream& outputStream();

private:

  liber::rsync::SegmentAccessor& mrAccessor;
  std::ofstream mOStream;

  ExecutionStatus mStatus;

  int mnInstructionTimeoutMs;
};


} // End namespace rsync
} // End namespace liber

#endif // RSYNC_ASSEMBLER_H
