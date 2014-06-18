#ifndef RSYNC_INSTRUCTION_QUEUE_H
#define RSYNC_INSTRUCTION_QUEUE_H

#include "Queue.h"
#include "InstructionReceiver.h"

namespace liber {
namespace rsync {

class InstructionQueue : public liber::rsync::InstructionReceiver {
public:

  InstructionQueue();
  ~InstructionQueue();

  /**
   * Transfer control of a Segment instance to the
   * receiver.
   */
  void push(Instruction* pInstruction);

  Instruction* pop(int nTimeoutMs);

  const char* toString() const
  {
    return "InstructionQueue";
  }
  
private:

  InstructionQueue(const InstructionQueue&);
  InstructionQueue& operator= (const InstructionQueue&);

private:

  Queue<Instruction*> mInstructions;

};

} // End namespace rsync
} // End namespace liber

#endif // RSYNC_INSTRUCTION_QUEUE_H
