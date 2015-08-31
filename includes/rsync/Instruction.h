#ifndef RSYNC_INSTRUCTION_H
#define RSYNC_INSTRUCTION_H

#include <fstream>
#include "JobDescriptor.h"
#include "Segment.h"
#include "SegmentAccessor.h"
#include "Serializable.h"
#include "RsyncError.h"

namespace liber {
namespace rsync {


class AssemblerState {
public:

  AssemblerState( SegmentAccessor& accessor );

  std::ofstream& stream();

  SegmentAccessor& segmentAccessor();

  JobDescriptor&   descriptor();
  void setDescriptor( JobDescriptor& job_descriptor );

  /**
   * Returns true if execution of an instruction failed or if the instruction
   * stream has been cancelled by the Authority.
   */
  bool failed() const;

  /**
   * Returns true if the instruction stream has been cancelled by the Authority.
   */
  bool cancelled() const;

  /**
   * Returns true if the instruction stream completed successfully.
   */
  bool done() const;

  /**
   *
   */
  void reset();


  RsyncError  status_;

  bool done_;

private:

  std::ofstream     stream_;

  SegmentAccessor&  segment_accessor_;

  JobDescriptor     job_descriptor_;
};


class Instruction : public liber::netapp::Serializable {
public:

  Instruction(ui32 type);
  virtual ~Instruction();

  ui32 type() const;


protected:

  ui32 mType;
};

class InstructionContainer : public liber::netapp::Serializable {
public:

  enum { kInvalidInstructionType = -1 };

  InstructionContainer( i32 type = kInvalidInstructionType );

  i32 type() const { return type_; };

  liber::netapp::SerialStream& stream() { return stream_; };

protected:

  void pack(liber::netapp::SerialStream& ctor) const;
  void pack(liber::netapp::SerialStream& ctor);
  bool unpack(liber::netapp::SerialStream& dtor);

private:

  liber::netapp::SerialStream stream_;

  i32 type_;
};


class BeginInstruction : public Instruction {
public:

  static const ui32 Type = 0;

  BeginInstruction();
  BeginInstruction(liber::rsync::JobDescriptor&);
  ~BeginInstruction();

  liber::rsync::JobDescriptor& descriptor(){ return descriptor_; };


protected:

  void pack(liber::netapp::SerialStream& ctor) const;
  void pack(liber::netapp::SerialStream& ctor);
  bool unpack(liber::netapp::SerialStream& dtor);

private:

  liber::rsync::JobDescriptor descriptor_;
};


class SegmentInstruction : public Instruction {
public:

  static const ui32 Type = 1;

  SegmentInstruction();
  SegmentInstruction(liber::rsync::Segment::ID id);
  ~SegmentInstruction();

  liber::rsync::Segment::ID getSegmentId() const;

  liber::rsync::Segment::ID segment_id_;

protected:

   void pack(liber::netapp::SerialStream& ctor) const;
   void pack(liber::netapp::SerialStream& ctor);
   bool unpack(liber::netapp::SerialStream& dtor);

};


class ChunkInstruction : public Instruction {
public:

  static const ui32 Type = 2;

  ChunkInstruction();
  ChunkInstruction(ui32 nSizeBytes);
  ~ChunkInstruction();

  ui8* const data();
  ui32 size() const;

  ui8* chunk_data_ptr_;
  ui32 chunk_size_bytes_;

protected:

  void pack(liber::netapp::SerialStream& ctor) const;
  void pack(liber::netapp::SerialStream& ctor);
  bool unpack(liber::netapp::SerialStream& dtor);

};


class EndInstruction : public Instruction {
public:

  static const ui32 Type = 3;

  EndInstruction();
  ~EndInstruction();

  void cancel(RsyncError error);
  bool canceled() const;
  RsyncError error() const;

  bool canceled_;

  // If cancelled, error that caused cancellation.
  ui32 cancel_error_;

protected:

  void pack(liber::netapp::SerialStream& ctor) const;
  void pack(liber::netapp::SerialStream& ctor);
  bool unpack(liber::netapp::SerialStream& dtor);

};

} // End namespace rsync
} // End namespace liber


#endif // RSYNC_INSTRUCTION_H

