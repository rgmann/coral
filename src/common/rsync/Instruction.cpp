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

const ui32 BeginInstruction::Type;
const ui32 SegmentInstruction::Type;
const ui32 ChunkInstruction::Type;
const ui32 EndInstruction::Type;

//-----------------------------------------------------------------------------
AssemblerState::AssemblerState( SegmentAccessor& accessor )
: segment_accessor_( accessor )
{
  reset();
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
JobDescriptor* AssemblerState::descriptor()
{
  return job_descriptor_ptr_;
}

//-----------------------------------------------------------------------------
void AssemblerState::setDescriptor( JobDescriptor& job_descriptor )
{
  job_descriptor_ptr_ = &job_descriptor;
}

//-----------------------------------------------------------------------------
bool AssemblerState::failed() const
{
  return (status_ != kRsyncSuccess);
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
  status_ = kRsyncSuccess;
  done_   = false;
  job_descriptor_ptr_ = NULL;
}

// //-----------------------------------------------------------------------------
// // Class: Instruction
// //-----------------------------------------------------------------------------
// Instruction::Instruction( ui32 type )
// : type_( type )
// {
// }

// //-----------------------------------------------------------------------------
// Instruction::~Instruction()
// {
// }

// //-----------------------------------------------------------------------------
// ui32 Instruction::type() const
// {
//    return type_;
// }

// //-----------------------------------------------------------------------------
// InstructionContainer::InstructionContainer( i32 type )
// :  type_( type )
// {
// }

// //-----------------------------------------------------------------------------
// InstructionContainer::InstructionContainer( const Instruction& instruction )
// :  type_( instruction.type() )
// {
//    serialize( stream() );
//    instruction.serialize( stream() );
// }

// //-----------------------------------------------------------------------------
// void InstructionContainer::pack(SerialStream& ctor) const
// {
//    ctor.write( (ui32)type_ );
// }

// //-----------------------------------------------------------------------------
// void InstructionContainer::pack(SerialStream& ctor)
// {
//    ctor.write( (ui32)type_ );
// }

// //-----------------------------------------------------------------------------
// bool InstructionContainer::unpack(SerialStream& dtor)
// {
//    if ( dtor.read( type_ ) == false )
//    {
//       type_ = kInvalidInstructionType;
//    }

//    return ( type_ != kInvalidInstructionType );
// }


// //-----------------------------------------------------------------------------
// // Class: BeginInstruction
// //-----------------------------------------------------------------------------
// BeginInstruction::BeginInstruction()
// :  Instruction( BeginInstruction::Type )
// {
// }

// //-----------------------------------------------------------------------------
// BeginInstruction::BeginInstruction( JobDescriptor& descriptor )
// :  Instruction( BeginInstruction::Type )
// ,  descriptor_( descriptor )
// {
// }

// //-----------------------------------------------------------------------------
// BeginInstruction::~BeginInstruction()
// {
// }

// //-----------------------------------------------------------------------------
// void BeginInstruction::pack( SerialStream& ctor ) const
// {
//   descriptor_.serialize( ctor );
// }

// //-----------------------------------------------------------------------------
// void BeginInstruction::pack( SerialStream& ctor )
// {
//   const_cast<const BeginInstruction*>(this)->pack( ctor );
// }

// //-----------------------------------------------------------------------------
// bool BeginInstruction::unpack( SerialStream& dtor )
// {
//   return descriptor_.deserialize( dtor );
// }

// //-----------------------------------------------------------------------------
// // Class: SegmentInstruction
// //-----------------------------------------------------------------------------
// SegmentInstruction::SegmentInstruction()
// : Instruction( SegmentInstruction::Type )
// {
// }

// //-----------------------------------------------------------------------------
// SegmentInstruction::SegmentInstruction( Segment::ID id )
// : Instruction( SegmentInstruction::Type )
// , segment_id_( id )
// {
// }

// //-----------------------------------------------------------------------------
// SegmentInstruction::~SegmentInstruction()
// {
// }

// //-----------------------------------------------------------------------------
// void SegmentInstruction::pack( SerialStream& ctor ) const
// {
//    ctor.write( (ui32)segment_id_ );
// }

// //-----------------------------------------------------------------------------
// void SegmentInstruction::pack( liber::netapp::SerialStream& ctor )
// {
//   const_cast<const SegmentInstruction*>(this)->pack(ctor);
// }

// //-----------------------------------------------------------------------------
// bool SegmentInstruction::unpack(SerialStream& dtor)
// {
//    bool unpack_success = true;
//    unpack_success     &= dtor.read( segment_id_ );
//    return unpack_success;
// }

// //-----------------------------------------------------------------------------
// // Class: SegmentInstruction
// //-----------------------------------------------------------------------------
// ChunkInstruction::ChunkInstruction()
// :  Instruction( ChunkInstruction::Type )
// ,  chunk_data_ptr_   ( NULL )
// ,  chunk_size_bytes_ ( 0 )
// {
// }

// //-----------------------------------------------------------------------------
// ChunkInstruction::ChunkInstruction( ui32 chunk_size_bytes )
// :  Instruction( ChunkInstruction::Type )
// ,  chunk_data_ptr_   ( NULL )
// ,  chunk_size_bytes_ ( chunk_size_bytes )
// {
//    if ( chunk_size_bytes > 0 )
//    {
//       chunk_data_ptr_ = new ui8[ chunk_size_bytes_ ];
//    }
// }

// //-----------------------------------------------------------------------------
// ChunkInstruction::~ChunkInstruction()
// {
//    if ( chunk_data_ptr_ )
//    {
//       delete[] chunk_data_ptr_;
//       chunk_data_ptr_ = NULL;
//    }
// }

// //-----------------------------------------------------------------------------
// ui8* const ChunkInstruction::data()
// {
//    return chunk_data_ptr_;
// }

// //-----------------------------------------------------------------------------
// ui32 ChunkInstruction::size() const
// {
//    return chunk_size_bytes_;
// }

// //-----------------------------------------------------------------------------
// void ChunkInstruction::pack(SerialStream& ctor) const
// {
//    if (chunk_data_ptr_)
//    {
//       std::string data;
//       data.assign( (char*)chunk_data_ptr_, chunk_size_bytes_ );
//       ctor.write( data );
//    }
// }

// //-----------------------------------------------------------------------------
// void ChunkInstruction::pack( SerialStream& ctor )
// {
//    const_cast<const ChunkInstruction*>(this)->pack(ctor);
// }

// //-----------------------------------------------------------------------------
// bool ChunkInstruction::unpack( SerialStream& dtor )
// {
//    bool unpack_success = false;
//    std::string data;

//    if ( ( dtor.read(data) == SerialStream::ReadOk ) && ( data.size() > 0 ) )
//    {
//       if ( chunk_data_ptr_ )
//       {
//          delete[] chunk_data_ptr_;
//          chunk_data_ptr_ = NULL;
//       }

//       chunk_size_bytes_ = data.size();
//       chunk_data_ptr_   = new ui8[ chunk_size_bytes_ ];

//       memcpy(chunk_data_ptr_, data.data(), data.size());

//       unpack_success = true;
//    }

//    return unpack_success;
// }


// //-----------------------------------------------------------------------------
// // Class: EndInstruction
// //-----------------------------------------------------------------------------
// EndInstruction::EndInstruction()
// :  Instruction( EndInstruction::Type )
// ,  canceled_( false )
// ,  cancel_error_( (RsyncError)kRsyncSuccess )
// {
// }

// //-----------------------------------------------------------------------------
// EndInstruction::~EndInstruction()
// {
// }

// //-----------------------------------------------------------------------------
// void EndInstruction::cancel( RsyncError error )
// {
//    canceled_     = true;
//    cancel_error_ = (ui32)error;
// }

// //-----------------------------------------------------------------------------
// bool EndInstruction::canceled() const
// {
//    return canceled_;
// }

// //-----------------------------------------------------------------------------
// RsyncError EndInstruction::error() const
// {
//    return (RsyncError)cancel_error_;
// }

// //-----------------------------------------------------------------------------
// void EndInstruction::pack( SerialStream& ctor ) const
// {
//    ctor.write( canceled_ );
//    ctor.write( cancel_error_ );
// }

// //-----------------------------------------------------------------------------
// void EndInstruction::pack( SerialStream& ctor )
// {
//    const_cast<const EndInstruction*>(this)->pack(ctor);
// }

// //-----------------------------------------------------------------------------
// bool EndInstruction::unpack( SerialStream& dtor )
// {
//    bool unpack_success = true;
//    unpack_success     &= dtor.read( canceled_ );
//    unpack_success     &= dtor.read( cancel_error_ );
//    return unpack_success;
// }

