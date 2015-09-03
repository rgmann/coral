#ifndef INSTRUCTION_LIST_H
#define INSTRUCTION_LIST_H

#include "InstructionReceiver.h"

namespace liber {
namespace rsync {

// Common interface expected by Segmenter.
class InstructionList : public liber::rsync::InstructionReceiver {
public:

  InstructionList();
  ~InstructionList();

  /**
   * Transfer control of a Segment instance to the
   * receiver.
   */
  void push( InstructionRaw* container_ptr );

  std::vector<InstructionRaw*>& instructions();

  
private:

  std::vector<InstructionRaw*> containers_;

};

} // End namespace rsync
} // End namespace liber

#endif // INSTRUCTION_LIST_H
