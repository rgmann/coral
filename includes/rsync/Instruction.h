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

  JobDescriptor&   jobDescriptor();

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

  virtual std::string toString() const = 0;

  virtual void execute( AssemblerState& state ) = 0;

protected:

  ui32 mType;
};


class BeginInstruction : public Instruction {
public:

  static const ui32 Type = 0;

  BeginInstruction();
  BeginInstruction(liber::rsync::JobDescriptor&);
  ~BeginInstruction();

  liber::rsync::JobDescriptor& getDescriptor();

  std::string toString() const;

  void execute( AssemblerState& state );

protected:

  void pack(liber::netapp::SerialStream& ctor) const;
  bool unpack(liber::netapp::SerialStream& dtor);

private:

  liber::rsync::JobDescriptor mDescriptor;
};


class SegmentInstruction : public Instruction {
public:

  static const ui32 Type = 1;

  SegmentInstruction();
  SegmentInstruction(liber::rsync::Segment::ID id);
  ~SegmentInstruction();

  liber::rsync::Segment::ID getSegmentId() const;

  std::string toString() const;
  void execute( AssemblerState& state );

protected:

  void pack(liber::netapp::SerialStream& ctor) const;
  bool unpack(liber::netapp::SerialStream& dtor);

private:

  liber::rsync::Segment::ID mID;
};


class ChunkInstruction : public Instruction {
public:

  static const ui32 Type = 2;

  ChunkInstruction();
  ChunkInstruction(ui32 nSizeBytes);
  ~ChunkInstruction();

  ui8* const data();
  ui32 size() const;

  std::string toString() const;
  void execute( AssemblerState& state );

protected:

  void pack(liber::netapp::SerialStream& ctor) const;
  bool unpack(liber::netapp::SerialStream& dtor);

private:

  ui8* chunk_data_ptr_;
  ui32 chunk_size_bytes_;
};


class EndInstruction : public Instruction {
public:

  static const ui32 Type = 3;

  EndInstruction();
  ~EndInstruction();

  void cancel(RsyncError error);
  bool cancelled() const;
  RsyncError error() const;

  std::string toString() const;
  void execute( AssemblerState& state );

protected:

  void pack(liber::netapp::SerialStream& ctor) const;
  bool unpack(liber::netapp::SerialStream& dtor);

private:

  bool mbCancelled;

  // If cancelled, error that caused cancellation.
  ui32 mCancelError;
};

class InstructionFactory {
public:

  static std::string Serialize(const Instruction* pInstruction);
  static Instruction* Deserialize(const std::string& data);
};



} // End namespace rsync
} // End namespace liber


#endif // RSYNC_INSTRUCTION_H

