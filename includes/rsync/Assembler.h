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

   void processBeginInstruction( liber::netapp::SerialStream& stream );
   void processChunkInstruction( liber::netapp::SerialStream& stream );
   void processSegmentInstruction( liber::netapp::SerialStream& stream );
   void processEndInstruction( liber::netapp::SerialStream& stream );

   Assembler( const Assembler& );
   Assembler& operator= ( const Assembler& );

private:

   AssemblerState assembler_state_;

};


} // End namespace rsync
} // End namespace liber

#endif // RSYNC_ASSEMBLER_H
