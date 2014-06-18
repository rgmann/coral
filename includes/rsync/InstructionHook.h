#ifndef INSTRUCTION_HOOK_H
#define INSTRUCTION_HOOK_H

#include "InstructionReceiver.h"

namespace liber {
namespace rsync {

// Common interface expected by Segmenter.
class InstructionHook : public liber::rsync::InstructionReceiver {
public:

  InstructionHook();
  ~InstructionHook();

  /**
   * Transfer control of a Segment instance to the
   * receiver.
   */
  void push(Instruction* pInstruction);

  virtual void call(Instruction* pInstruction) = 0;

  const char* toString() const
  {
    return "InstructionHook";
  }
};

} // End namespace rsync
} // End namespace liber

#endif // INSTRUCTION_HOOK_H
