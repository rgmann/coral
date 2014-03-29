#ifndef INSTRUCTION_RECEIVER_H
#define INSTRUCTION_RECEIVER_H

#include "Instruction.h"

namespace liber {
namespace rsync {

// Common interface expected by Segmenter.
class InstructionReceiver {
public:

  InstructionReceiver();
  virtual ~InstructionReceiver();

  /**
   * Transfer control of a Segment instance to the
   * receiver.
   */
  virtual void push(Instruction* const pInstruction) = 0;

};

} // End namespace rsync
} // End namespace liber

#endif // INSTRUCTION_RECEIVER_H
