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
  void push( RawInstructionPtr container_ptr );

  RawInstructionPtr pop(int nTimeoutMs);
  
private:

  InstructionQueue( const InstructionQueue& );
  InstructionQueue& operator= ( const InstructionQueue& );

private:

  Queue<RawInstructionPtr> containers_;

};

} // End namespace rsync
} // End namespace liber

#endif // RSYNC_INSTRUCTION_QUEUE_H
