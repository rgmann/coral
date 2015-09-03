#ifndef INSTRUCTION_HOOK_H
#define INSTRUCTION_HOOK_H

#include "InstructionReceiver.h"

namespace liber {
namespace rsync {

// Common interface expected by Segmenter.
class InstructionHook : public liber::rsync::InstructionReceiver {
public:

  InstructionHook();
  virtual ~InstructionHook();

  /**
   * Transfer control of a Segment instance to the
   * receiver.
   */
  void push( InstructionRaw* instruction_ptr );


protected:

  virtual void call( InstructionRaw* instruction_ptr ) = 0;
};

} // End namespace rsync
} // End namespace liber

#endif // INSTRUCTION_HOOK_H
