#ifndef RSYNC_ASSEMBLER_H
#define RSYNC_ASSEMBLER_H

#include <fstream>

namespace liber {
namespace rsync {

class SegmentAccessor;
class InstructionQueue;
class AssemblyReport;

class Assembler {
public:

  Assembler( SegmentAccessor& segment_accessor );
  ~Assembler();

  bool process( InstructionQueue& instruction_queue, AssemblyReport& report );

  //
  // Get assembly status for the last completed job.
  //
  const ExecutionStatus& status() const;

  std::ofstream& outputStream();

private:

  Assembler( const Assembler& );
  Assembler& operator= ( const Assembler& );

private:

  SegmentAccessor&  segment_accessor_;
  std::ofstream output_stream_;

  ExecutionStatus assembly_status_;

  int instruction_timeout_ms_;
};


} // End namespace rsync
} // End namespace liber

#endif // RSYNC_ASSEMBLER_H
