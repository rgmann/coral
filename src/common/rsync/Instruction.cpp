#include <sstream>
#include "Log.h"
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
// Class: ExecutionStatus
//-----------------------------------------------------------------------------
ExecutionStatus::ExecutionStatus()
{
  reset();
}

//-----------------------------------------------------------------------------
ExecutionStatus::~ExecutionStatus()
{
}

//-----------------------------------------------------------------------------
bool ExecutionStatus::failed() const
{
  return (error != NoError);
}

//-----------------------------------------------------------------------------
bool ExecutionStatus::cancelled() const
{
  return (error == CancelError);
}

//-----------------------------------------------------------------------------
bool ExecutionStatus::done() const
{
  return mbDone;
}

//-----------------------------------------------------------------------------
void ExecutionStatus::reset()
{
  error = ExecutionStatus::NoError;
  mbDone = false;
}

//-----------------------------------------------------------------------------
// Class: Instruction
//-----------------------------------------------------------------------------
std::string InstructionFactory::Serialize(const Instruction* pInstruction)
{
  SerialStream ctor;

  ctor.write(pInstruction->type());
  pInstruction->serialize(ctor);

//  dump_buffer(ctor.stream.str().data(), ctor.stream.str().size());

  return ctor.stream.str();
}
 
//-----------------------------------------------------------------------------
Instruction* InstructionFactory::Deserialize(const std::string& data)
{
  Instruction* lpInstruction = NULL;

  SerialStream dtor;
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
ui32 Instruction::type() const
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
     << "  path: " << mDescriptor.getDestination().path.generic_string() << std::endl
     << "  segment size: " << mDescriptor.getSegmentSize() << std::endl;

  return ss.str();
}

//-----------------------------------------------------------------------------
void BeginInstruction::
execute(ExecutionStatus& rStatus, SegmentAccessor& rAccessor, std::ofstream& ostream)
{
  if (ostream.is_open()) return;

  ostream.open(mDescriptor.getDestination().path.string().c_str(),
               std::ofstream::binary);
  if ((ostream.is_open() == false) || ostream.fail())
  {
    rStatus.error = ExecutionStatus::IoError;
    log::error("BeginInstruction: Failed ostream or not open.\n");
  }
}

//-----------------------------------------------------------------------------
void BeginInstruction::pack(SerialStream& ctor) const
{
  mDescriptor.serialize(ctor);
  //ctor.write(mDescriptor.getSegmentSize());
  //ctor.writeCString(mDescriptor.getDestination().path.generic_string());
}

//-----------------------------------------------------------------------------
bool BeginInstruction::unpack(SerialStream& dtor)
{
/*  bool lbSuccess = true;
  std::cout << "BeginInstruction::unpack " << std::endl;

  ui32 lnSegmentSize;
  lbSuccess &= dtor.read(lnSegmentSize);

  std::string lPath;
  lbSuccess &= dtor.readCString(lPath);

  mDescriptor = JobDescriptor(lPath, lnSegmentSize);

  return lbSuccess;*/
  return mDescriptor.deserialize(dtor);
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
void SegmentInstruction::
execute(ExecutionStatus& rStatus, SegmentAccessor& rAccessor, std::ofstream& ostream)
{
  Segment* lpSegment = rAccessor.getSegment(mID);
  if (lpSegment)
  {
    ostream.write((char*)lpSegment->data(), lpSegment->size());

    if (ostream.fail())
    {
      rStatus.error = ExecutionStatus::IoError;
      log::error("SegmentInstruction: Failed to write segment to ostream.\n");
    }
#ifdef DEBUG_RSYNC_INSTRUCTIONS
    log::debug("Executing SegmentInstruction:\n%s\n",
               lpSegment->debugDump().c_str());
#endif
  }
  else
  {
    rStatus.error = ExecutionStatus::SegmentAccessError;
    log::error("SegmentInstruction: Failed to access segment.\n");
  }
}

//-----------------------------------------------------------------------------
void SegmentInstruction::pack(SerialStream& ctor) const
{
  ctor.write((ui32)mID);
}

//-----------------------------------------------------------------------------
bool SegmentInstruction::unpack(SerialStream& dtor)
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
void ChunkInstruction::
execute(ExecutionStatus& rStatus, SegmentAccessor& rAccessor, std::ofstream& ostream)
{
  if (mpData && (mnSizeBytes > 0))
  {
    ostream.write((char*)mpData, mnSizeBytes);
#ifdef DEBUG_RSYNC_INSTRUCTIONS
    log::debug("Executing ChunkInstruction:\n");
    for (int ind = 0; ind < mnSizeBytes; ind++)
    {
      printf("0x%02X ", mpData[ind]);
      if ((ind > 0) && (ind % 16 == 0)) printf("\n");
    }
    printf("\n");
#endif
  }

  if ((mpData == NULL) || ostream.fail())
  {
    rStatus.error = ExecutionStatus::IoError;
    log::error("ChunkInstruction: NULL data or failed ostream.\n");
  }
}

//-----------------------------------------------------------------------------
void ChunkInstruction::pack(SerialStream& ctor) const
{
  if (mpData)
  {
    std::string data;
    data.assign((char*)mpData, mnSizeBytes);
    ctor.write(data);
  }
}

//-----------------------------------------------------------------------------
bool ChunkInstruction::unpack(SerialStream& dtor)
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
, mbCancelled(false)
, mCancelError((RsyncError)RsyncSuccess)
{
}

//-----------------------------------------------------------------------------
EndInstruction::~EndInstruction()
{
}

//-----------------------------------------------------------------------------
void EndInstruction::cancel(RsyncError error)
{
  mbCancelled = true;
  mCancelError = (ui32)error;
}

//-----------------------------------------------------------------------------
bool EndInstruction::cancelled() const
{
  return mbCancelled;
}

//-----------------------------------------------------------------------------
RsyncError EndInstruction::error() const
{
  return (RsyncError)mCancelError;
}

//-----------------------------------------------------------------------------
std::string EndInstruction::toString() const
{
  return std::string("EndInstruction");
}

//-----------------------------------------------------------------------------
void EndInstruction::
execute(ExecutionStatus& rStatus, SegmentAccessor& rAccessor, std::ofstream& ostream)
{
  if (mbCancelled)
  {
    rStatus.error = ExecutionStatus::CancelError;
  }
  else
  {
    rStatus.mbDone = true;
  }

  ostream.close();
}

//-----------------------------------------------------------------------------
void EndInstruction::pack(SerialStream& ctor) const
{
  ctor.write(mbCancelled);
  ctor.write(mCancelError);
}

//-----------------------------------------------------------------------------
bool EndInstruction::unpack(SerialStream& dtor)
{
  bool lbSuccess = true;
  lbSuccess &= dtor.read(mbCancelled);
  lbSuccess &= dtor.read(mCancelError);
  return lbSuccess;
}

