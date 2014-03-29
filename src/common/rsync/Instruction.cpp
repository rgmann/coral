#include <sstream>
#include "Instruction.h"

#undef DEBUG_RSYNC_INSTRUCTIONS

using namespace liber::rsync;
using namespace liber::netapp;

void dump_buffer(const char* pBuffer, ui32 nBytes)
{
  for (int nByte = 0; nByte < nBytes; nByte++)
  {
    printf(" %02X", pBuffer[nByte]);
    if ((nByte > 0) && (nByte % 16 == 0)) printf("\n");
  }
  printf("\n");
}

//-----------------------------------------------------------------------------
// Class: Instruction
//-----------------------------------------------------------------------------
std::string InstructionFactory::Serialize(const Instruction* pInstruction)
{
  PacketCtor ctor;

  ctor.write(pInstruction->getType());
  pInstruction->serialize(ctor);

//  dump_buffer(ctor.stream.str().data(), ctor.stream.str().size());

  return ctor.stream.str();
}
 
//-----------------------------------------------------------------------------
Instruction* InstructionFactory::Deserialize(const std::string& data)
{
  Instruction* lpInstruction = NULL;

  PacketDtor dtor;
  dtor.stream.write(data.data(), data.size());

  ui32 lType;
  if (dtor.read(lType) == false)
  {
    return NULL;
  }

  switch (lType)
  {
  case BeginInstruction::Type:
    lpInstruction = new BeginInstruction();
    break;
  case SegmentInstruction::Type:
    lpInstruction = new SegmentInstruction();
    break;
  case ChunkInstruction::Type:
    lpInstruction = new ChunkInstruction();
    break;
  case EndInstruction::Type:
    lpInstruction = new EndInstruction();
    break;
  default: break;
  }

  if (lpInstruction)
  {
    lpInstruction->deserialize(dtor);
  }

  return lpInstruction;
}

//-----------------------------------------------------------------------------
// Class: Instruction
//-----------------------------------------------------------------------------
Instruction::Instruction(ui32 type)
: mType(type)
{
}

//-----------------------------------------------------------------------------
Instruction::~Instruction()
{
}

//-----------------------------------------------------------------------------
ui32 Instruction::getType() const
{
  return mType;
}


//-----------------------------------------------------------------------------
// Class: BeginInstruction
//-----------------------------------------------------------------------------
BeginInstruction::BeginInstruction()
: Instruction(BeginInstruction::Type)
{
}

//-----------------------------------------------------------------------------
BeginInstruction::BeginInstruction(JobDescriptor& descriptor)
: Instruction(BeginInstruction::Type)
, mDescriptor(descriptor)
{
}

//-----------------------------------------------------------------------------
BeginInstruction::~BeginInstruction()
{
}

//-----------------------------------------------------------------------------
std::string BeginInstruction::toString() const
{
  std::stringstream ss;

  ss << "BeginInstruction:" << std::endl
     << "  path: " << mDescriptor.getPath().generic_string() << std::endl
     << "  segment size: " << mDescriptor.getSegmentSize() << std::endl;

  return ss.str();
}

//-----------------------------------------------------------------------------
bool BeginInstruction::
execute(SegmentAccessor& rAccessor, std::ofstream& ostream)
{
  if (ostream.is_open()) return false;
  ostream.open(mDescriptor.getPath().generic_string(), std::ofstream::binary);
  return ostream.is_open();
}

//-----------------------------------------------------------------------------
void BeginInstruction::serialize(PacketCtor& ctor) const
{
  ctor.write(mDescriptor.getSegmentSize());
  ctor.writeCString(mDescriptor.getPath().generic_string());
}

//-----------------------------------------------------------------------------
bool BeginInstruction::deserialize(PacketDtor& dtor)
{
  bool lbSuccess = true;
  std::cout << "BeginInstruction::deserialize " << std::endl;

  ui32 lnSegmentSize;
  lbSuccess &= dtor.read(lnSegmentSize);

  std::string lPath;
  lbSuccess &= dtor.readCString(lPath);

  mDescriptor = JobDescriptor(lPath, lnSegmentSize);

  return lbSuccess;
}

//-----------------------------------------------------------------------------
// Class: SegmentInstruction
//-----------------------------------------------------------------------------
SegmentInstruction::SegmentInstruction()
: Instruction(SegmentInstruction::Type)
{
}

//-----------------------------------------------------------------------------
SegmentInstruction::SegmentInstruction(Segment::ID id)
: Instruction(SegmentInstruction::Type)
, mID(id)
{
}

//-----------------------------------------------------------------------------
SegmentInstruction::~SegmentInstruction()
{
}

//-----------------------------------------------------------------------------
std::string SegmentInstruction::toString() const
{
  std::stringstream ss;

  ss << "SegmentInstruction:" << std::endl
     << "  ID: " << mID << std::endl;

  return ss.str();
}

//-----------------------------------------------------------------------------
bool SegmentInstruction::
execute(SegmentAccessor& rAccessor, std::ofstream& ostream)
{
  Segment* lpSegment = rAccessor.getSegment(mID);
  if (lpSegment)
  {
    ostream.write((char*)lpSegment->data(), lpSegment->size());

#ifdef DEBUG_RSYNC_INSTRUCTIONS
    std::cout << "Executing SegmentInstruction:" << std::endl
              << " " << lpSegment->debugDump() << std::endl;
#endif
  }
  return (lpSegment && (ostream.fail() == false));
}

//-----------------------------------------------------------------------------
void SegmentInstruction::serialize(PacketCtor& ctor) const
{
  ctor.write((ui32)mID);
}

//-----------------------------------------------------------------------------
bool SegmentInstruction::deserialize(PacketDtor& dtor)
{
  bool lbSuccess = true;
  ui32 id;
  lbSuccess &= dtor.read(id);
  mID = id;
  return lbSuccess;
}

//-----------------------------------------------------------------------------
// Class: SegmentInstruction
//-----------------------------------------------------------------------------
ChunkInstruction::ChunkInstruction()
: Instruction(ChunkInstruction::Type)
, mpData(NULL)
, mnSizeBytes(0)
{
}

//-----------------------------------------------------------------------------
ChunkInstruction::ChunkInstruction(ui32 nSizeBytes)
: Instruction(ChunkInstruction::Type)
, mpData(NULL)
, mnSizeBytes(nSizeBytes)
{
  if (nSizeBytes > 0)
  {
    mpData = new ui8[mnSizeBytes];
  }
}

//-----------------------------------------------------------------------------
ChunkInstruction::~ChunkInstruction()
{
  if (mpData)
  {
    delete[] mpData;
    mpData = NULL;
  }
}

//-----------------------------------------------------------------------------
ui8* const ChunkInstruction::data()
{
  return mpData;
}

//-----------------------------------------------------------------------------
ui32 ChunkInstruction::size() const
{
  return mnSizeBytes;
}

//-----------------------------------------------------------------------------
std::string ChunkInstruction::toString() const
{
  std::stringstream ss;

  ss << "ChunkInstruction:" << std::endl
     << "  chunk size: " << mnSizeBytes << std::endl;
#ifdef DEBUG_RSYNC_INSTRUCTIONS
  for (int nByte = 0; nByte < mnSizeBytes; nByte++)
  {
    printf(" %02X", mpData[nByte]);
    if (nByte > 0 && (nByte % 16 == 0)) printf("\n");
  }
  printf("\n");
#endif

  return ss.str();
}

//-----------------------------------------------------------------------------
bool ChunkInstruction::
execute(SegmentAccessor& rAccessor, std::ofstream& ostream)
{
  if (mpData && (mnSizeBytes > 0))
  {
    ostream.write((char*)mpData, mnSizeBytes);
#ifdef DEBUG_RSYNC_INSTRUCTIONS
    printf("Executing ChunkInstruction:\n");
    for (int ind = 0; ind < mnSizeBytes; ind++)
    {
      printf("0x%02X ", mpData[ind]);
      if ((ind > 0) && (ind % 16 == 0)) printf("\n");
    }
    printf("\n");
#endif
  }
  return (mpData && (ostream.fail() == false));
}

//-----------------------------------------------------------------------------
void ChunkInstruction::serialize(PacketCtor& ctor) const
{
  if (mpData)
  {
    std::string data;
    data.assign((char*)mpData, mnSizeBytes);
    ctor.write(data);
  }
}

//-----------------------------------------------------------------------------
bool ChunkInstruction::deserialize(PacketDtor& dtor)
{
  bool lbSuccess = true;
  std::string data;
  lbSuccess &= dtor.read(data);
  if (data.size() > 0)
  {
    if (mpData)
    {
      delete[] mpData;
      mpData = NULL;
    }

    mpData = new ui8[data.size()];
    mnSizeBytes = data.size();
    memcpy(mpData, data.data(), data.size());
  }
  return lbSuccess;
}


//-----------------------------------------------------------------------------
// Class: EndInstruction
//-----------------------------------------------------------------------------
EndInstruction::EndInstruction()
: Instruction(EndInstruction::Type)
{
}

//-----------------------------------------------------------------------------
EndInstruction::~EndInstruction()
{
}

//-----------------------------------------------------------------------------
std::string EndInstruction::toString() const
{
  return std::string("EndInstruction");
}

//-----------------------------------------------------------------------------
bool EndInstruction::
execute(SegmentAccessor& rAccessor, std::ofstream& ostream)
{
  ostream.close();
  return (ostream.is_open() == false);
}

//-----------------------------------------------------------------------------
void EndInstruction::serialize(PacketCtor& ctor) const
{
}

//-----------------------------------------------------------------------------
bool EndInstruction::deserialize(PacketDtor& dtor)
{
  bool lbSuccess = true;
  return lbSuccess;
}

