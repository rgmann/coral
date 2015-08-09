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
  Instruction* instruction_ptr = NULL;

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
      instruction_ptr = new BeginInstruction();
      break;
    case SegmentInstruction::Type:
      instruction_ptr = new SegmentInstruction();
      break;
    case ChunkInstruction::Type:
      instruction_ptr = new ChunkInstruction();
      break;
    case EndInstruction::Type:
      instruction_ptr = new EndInstruction();
      break;
    default: break;
  }

  if (instruction_ptr)
  {
    instruction_ptr->deserialize(dtor);
  }

  return instruction_ptr;
}

//-----------------------------------------------------------------------------
AssemblerState::AssemblerState( SegmentAccessor& accessor )
: segment_accessor_( accessor )
{
}

//-----------------------------------------------------------------------------
std::ofstream& AssemblerState::stream()
{
  return stream_;
}

//-----------------------------------------------------------------------------
SegmentAccessor& AssemblerState::segmentAccessor()
{
  return segment_accessor_;
}

//-----------------------------------------------------------------------------
JobDescriptor& AssemblerState::jobDescriptor()
{
  return job_descriptor_;
}

//-----------------------------------------------------------------------------
bool AssemblerState::failed() const
{
  return (status_ != RsyncSuccess);
}

//-----------------------------------------------------------------------------
bool AssemblerState::cancelled() const
{
  return (status_ == kRsyncJobCanceled);
}

//-----------------------------------------------------------------------------
bool AssemblerState::done() const
{
  return done_;
}

//-----------------------------------------------------------------------------
void AssemblerState::reset()
{
  status_ = RsyncSuccess;
  done_ = false;
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
     << "  path: " << mDescriptor.getDestination().path().generic_string() << std::endl
     << "  segment size: " << mDescriptor.getSegmentSize() << std::endl;

  return ss.str();
}

//-----------------------------------------------------------------------------
void BeginInstruction::execute( AssemblerState& state )
{
  if ( mDescriptor.getDestination().path() != 
       state.jobDescriptor().getDestination().path() )
  {
    state.status_ = kRsyncInvalidJob;
  }
}

//-----------------------------------------------------------------------------
void BeginInstruction::pack(SerialStream& ctor) const
{
  mDescriptor.serialize(ctor);
}

//-----------------------------------------------------------------------------
bool BeginInstruction::unpack(SerialStream& dtor)
{
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
void SegmentInstruction::execute( AssemblerState& state )
{
  Segment* segment_ptr = state.segmentAccessor().getSegment( mID );

  if ( segment_ptr != NULL )
  {
    state.stream().write( (char*)segment_ptr->data(), segment_ptr->size() );

    if ( state.stream().fail() )
    {
      state.status_ = kRsyncIoError;
      log::error("SegmentInstruction: Failed to write segment to ostream.\n");
    }
#ifdef DEBUG_RSYNC_INSTRUCTIONS
    log::debug("Executing SegmentInstruction:\n%s\n",
               segment_ptr->debugDump().c_str());
#endif
  }
  else
  {
    // rStatus.error = ExecutionStatus::SegmentAccessError;
    state.status_ = kRsyncSegmentAccessError;
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
: Instruction( ChunkInstruction::Type )
, chunk_data_ptr_   ( NULL )
, chunk_size_bytes_ ( 0 )
{
}

//-----------------------------------------------------------------------------
ChunkInstruction::ChunkInstruction( ui32 chunk_size_bytes )
: Instruction( ChunkInstruction::Type )
, chunk_data_ptr_   ( NULL )
, chunk_size_bytes_ ( chunk_size_bytes )
{
  if ( chunk_size_bytes > 0 )
  {
    chunk_data_ptr_ = new ui8[ chunk_size_bytes_ ];
  }
}

//-----------------------------------------------------------------------------
ChunkInstruction::~ChunkInstruction()
{
  if ( chunk_data_ptr_ )
  {
    delete[] chunk_data_ptr_;
    chunk_data_ptr_ = NULL;
  }
}

//-----------------------------------------------------------------------------
ui8* const ChunkInstruction::data()
{
  return chunk_data_ptr_;
}

//-----------------------------------------------------------------------------
ui32 ChunkInstruction::size() const
{
  return chunk_size_bytes_;
}

//-----------------------------------------------------------------------------
std::string ChunkInstruction::toString() const
{
  std::stringstream ss;

  ss << "ChunkInstruction:" << std::endl
     << "  chunk size: " << chunk_size_bytes_ << std::endl;
#ifdef DEBUG_RSYNC_INSTRUCTIONS
  for (int nByte = 0; nByte < chunk_size_bytes_; nByte++)
  {
    printf(" %02X", chunk_data_ptr_[nByte]);
    if (nByte > 0 && (nByte % 16 == 0)) printf("\n");
  }
  printf("\n");
#endif

  return ss.str();
}

//-----------------------------------------------------------------------------
void ChunkInstruction::execute( AssemblerState& state )
{
  if ( chunk_data_ptr_ )
  {

    // Validate the chunk size
    if ( ( chunk_size_bytes_ > 0 ) &&
         ( chunk_size_bytes_ <= state.jobDescriptor().getMaximumChunkSize() ) )
    {
      state.stream().write( (char*)chunk_data_ptr_, chunk_size_bytes_ );

#ifdef DEBUG_RSYNC_INSTRUCTIONS
      log::debug("Executing ChunkInstruction:\n");
      for (int ind = 0; ind < chunk_size_bytes_; ind++)
      {
        printf("0x%02X ", chunk_data_ptr_[ind]);
        if ((ind > 0) && (ind % 16 == 0)) printf("\n");
      }
      printf("\n");
#endif

      if ( state.stream().fail() )
      {
        state.status_ = kRsyncIoError;
      }
    }
    else
    {
      state.status_ = kRsyncAssemblerInvalidChunkSize;
    }
  }
  else
  {
    state.status_ = kRsyncIoError;
    log::error("ChunkInstruction: NULL data or failed ostream.\n");
  }
}

//-----------------------------------------------------------------------------
void ChunkInstruction::pack(SerialStream& ctor) const
{
  if (chunk_data_ptr_)
  {
    std::string data;
    data.assign((char*)chunk_data_ptr_, chunk_size_bytes_);
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
    if (chunk_data_ptr_)
    {
      delete[] chunk_data_ptr_;
      chunk_data_ptr_ = NULL;
    }

    chunk_data_ptr_ = new ui8[data.size()];
    chunk_size_bytes_ = data.size();
    memcpy(chunk_data_ptr_, data.data(), data.size());
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
void EndInstruction::execute( AssemblerState& state )
{
  if (mbCancelled)
  {
    state.status_ = kRsyncJobCanceled;
  }
  else
  {
    state.done_ = true;
  }

  state.stream().close();
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

