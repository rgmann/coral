#ifndef RSYNC_ASSEMBLER_H
#define RSYNC_ASSEMBLER_H

#include <fstream>

namespace liber {
namespace rsync {

class JobDescriptor;
class SegmentAccessor;
class InstructionQueue;
class AssemblyReport;

class Assembler {
public:

   Assembler( SegmentAccessor& segment_accessor );
   ~Assembler();

   bool process(
      JobDescriptor&    job_descriptor,
      InstructionQueue& instruction_queue,
      AssemblyReport&   report );

   std::ofstream& outputStream();

private:

   void processBeginInstruction( InstructionRaw* instruction_ptr );
   void processChunkInstruction( InstructionRaw* instruction_ptr );
   void processSegmentInstruction( InstructionRaw* instruction_ptr );
   void processEndInstruction( InstructionRaw* instruction_ptr );

   Assembler( const Assembler& );
   Assembler& operator= ( const Assembler& );

private:

   AssemblerState assembler_state_;

};


} // End namespace rsync
} // End namespace liber

#endif // RSYNC_ASSEMBLER_H
