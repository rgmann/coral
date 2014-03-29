#ifndef RSYNC_INSTRUCTION_H
#define RSYNC_INSTRUCTION_H

#include <fstream>
#include "Segment.h"
#include "SegmentAccessor.h"
#include "Segmenter.h"
#include "PacketHelper.h"

namespace liber {
namespace rsync {



class Instruction {
public:

  Instruction(ui32 type);
  virtual ~Instruction();

  ui32 getType() const;

  virtual std::string toString() const = 0;

  virtual bool execute(SegmentAccessor&, std::ofstream&) = 0;

  virtual void serialize(liber::netapp::PacketCtor& ctor) const = 0;
  virtual bool deserialize(liber::netapp::PacketDtor& dtor) = 0;

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

  bool execute(SegmentAccessor&, std::ofstream&);

  void serialize(liber::netapp::PacketCtor& ctor) const;
  bool deserialize(liber::netapp::PacketDtor& dtor);

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
  bool execute(SegmentAccessor&, std::ofstream&);

  void serialize(liber::netapp::PacketCtor& ctor) const;
  bool deserialize(liber::netapp::PacketDtor& dtor);

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
  bool execute(SegmentAccessor&, std::ofstream&);

  void serialize(liber::netapp::PacketCtor& ctor) const;
  bool deserialize(liber::netapp::PacketDtor& dtor);

private:

  ui8* mpData;
  ui32 mnSizeBytes;
};


class EndInstruction : public Instruction {
public:

  static const ui32 Type = 3;

  EndInstruction();
  ~EndInstruction();

  std::string toString() const;
  bool execute(SegmentAccessor&, std::ofstream&);

  void serialize(liber::netapp::PacketCtor& ctor) const;
  bool deserialize(liber::netapp::PacketDtor& dtor);

};

class InstructionFactory {
public:

  static std::string Serialize(const Instruction* pInstruction);
  static Instruction* Deserialize(const std::string& data);
};



} // End namespace rsync
} // End namespace liber


#endif // RSYNC_INSTRUCTION_H

