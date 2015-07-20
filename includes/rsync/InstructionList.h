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
  void push( Instruction* instruction_ptr );

  std::vector<Instruction*>& instructions();

  
private:

  std::vector<Instruction*> instructions_;

};

} // End namespace rsync
} // End namespace liber

#endif // INSTRUCTION_LIST_H
