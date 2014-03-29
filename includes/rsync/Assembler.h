#ifndef RSYNC_ASSEMBLER_H
#define RSYNC_ASSEMBLER_H

#include <fstream>
#include "Instruction.h"
#include "SegmentAccessor.h"

namespace liber {
namespace rsync {


class Assembler {
public:

  Assembler(SegmentAccessor&, std::ofstream&);
  ~Assembler();

  /**
   * Process the next assembly instruction.
   */
  bool execute(Instruction* pInstruction);

private:

  liber::rsync::SegmentAccessor& mrAccessor;
  std::ofstream& mrOStream;
};


} // End namespace rsync
} // End namespace liber

#endif // RSYNC_ASSEMBLER_H
