#ifndef RSYNC_INSTRUCTION_H
#define RSYNC_INSTRUCTION_H

#include <fstream>
#include <string.h>
#include "Log.h"
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

  JobDescriptor*   descriptor();
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

  JobDescriptor*    job_descriptor_ptr_;
};


// class Instruction : public liber::netapp::Serializable {
// public:

//   Instruction( ui32 type );
//   virtual ~Instruction();

//   ui32 type() const;


// protected:

//   ui32 type_;
// };

class InstructionRaw {
public:
  InstructionRaw( const void* data_ptr, ui32 length )
  :  data_ptr_ ( NULL )
  ,  length_ ( 0 )
  {
    // liber::log::status("FIRST\n");
    allocate( length );
    memcpy( data_ptr_, data_ptr, length_ );
  }
  InstructionRaw( ui32 type, ui32 length )
  :  data_ptr_ ( NULL )
  ,  length_ ( 0 )
  {
    // liber::log::status("SECOND\n");
    allocate( length + sizeof(i32) );
    i32* type_ptr = (i32*)data_ptr_;
    *type_ptr = type;
  }
  ~InstructionRaw() {
    if ( data_ptr_ != NULL ) delete[] data_ptr_;
  }

  enum { kInvalidInstructionType = -1 };
  i32 type() const {
    i32* type_ptr = NULL;
    // liber::log::status("InstructionRaw::type: data=%p, length=%d\n", data_ptr_, length_ );
    if ( length_ >= sizeof( i32 ) ) {
      type_ptr = (i32*)data_ptr_;
    }
    i32 type = kInvalidInstructionType;
    if ( type_ptr ) {
      type = *type_ptr;
    }
    return type;
  }
  ui32 length() const { return length_; }
  ui32 payload_length() const {
    ui32 payload_length = 0;
    if ( length_ > sizeof(i32)) payload_length = length_ - sizeof(i32);
    return payload_length;
  }
  void* const data() const { return data_ptr_; }
  void* const payload_ptr() const {
    if ( data_ptr_ && (length_ > sizeof(i32))) {
      return ((ui8*)data_ptr_ + sizeof(i32));
    }
    return NULL;
  }

  void allocate( ui32 length ) {
    if ( data_ptr_ != NULL ) {
      delete[] data_ptr_;
      data_ptr_ = NULL;
    }
    data_ptr_ = new ui8[ length ];
    length_ = length;
    memset( data_ptr_, 0, length_ );
  }

  bool valid() const {
    return (
      payload_ptr() &&
      ( type() != InstructionRaw::kInvalidInstructionType ) );
  }

  void dump() {
    if ( valid() ) {
      liber::log::mem_dump("", (const char*)data(), length() );
    }
  }

private:

  ui8* data_ptr_;
  ui32 length_;

};

typedef  std::shared_ptr<InstructionRaw> InstructionRawPtr;
class Instruction {
public:
  // Instuction() : instruction_ptr_( NULL ) {}
  Instruction( i32 type, InstructionRaw* instruction_ptr = NULL )
  : type_( type ), instruction_ptr_( instruction_ptr ) {}
  virtual ~Instruction() {
    if ( instruction_ptr_ != NULL ) delete instruction_ptr_;
  }

  bool valid() const {
    // if ( instruction_ptr_ )
      // liber::log::status("valid: %d, %d\n", instruction_ptr_->valid(), instruction_ptr_->type());
    return (
      ( instruction_ptr_ != NULL ) &&
      instruction_ptr_->valid() &&
      ( instruction_ptr_->type() == type_ ) );
  }

  i32 type() const {
    if ( valid() ) {
      return instruction_ptr_->type();
    } else {
      return InstructionRaw::kInvalidInstructionType;
    }
  }

  InstructionRaw* instruction(){ return instruction_ptr_; }
  void release() { instruction_ptr_ = NULL; }

protected:
  i32 type_;
  InstructionRaw* instruction_ptr_;
};

// class InstructionContainer : public liber::netapp::Serializable {
// public:

//   enum { kInvalidInstructionType = -1 };

//   InstructionContainer( i32 type = kInvalidInstructionType );
//   InstructionContainer( const Instruction& instruction );

//   i32 type() const { return type_; };

//   liber::netapp::SerialStream& stream() { return stream_; };

// protected:

//   void pack(liber::netapp::SerialStream& ctor) const;
//   void pack(liber::netapp::SerialStream& ctor);
//   bool unpack(liber::netapp::SerialStream& dtor);

// private:

//   liber::netapp::SerialStream stream_;

//   i32 type_;
// };


class BeginInstruction : public Instruction {
public:

  static const ui32 Type = 0;

  BeginInstruction( InstructionRaw* instruction_ptr = NULL ) : Instruction( Type, instruction_ptr ){}
  BeginInstruction(liber::rsync::JobDescriptor& descriptor ) : Instruction( Type, NULL ) {
    std::string packed = descriptor.serialize();
    // instruction_ptr_ = new InstructionRaw( packed.data(), packed.size() );
    instruction_ptr_ = new InstructionRaw( Type, packed.size() );
    memcpy( instruction_ptr_->payload_ptr(), packed.data(), packed.size() );
  }
  // ~BeginInstruction();

  bool descriptor( liber::rsync::JobDescriptor& descriptor ) {
    bool success = false;
    if ( instruction_ptr_ != NULL ) {
      success = descriptor.deserialize(
        (const char*)instruction_ptr_->payload_ptr(),
        instruction_ptr_->payload_length() );
      liber::log::status("BeginInstruction::descriptor: %d, %d, %d\n",
        instruction_ptr_->length(), instruction_ptr_->payload_length(), success);
    }
    return success;
  }


// protected:

//   void pack(liber::netapp::SerialStream& ctor) const;
//   void pack(liber::netapp::SerialStream& ctor);
//   bool unpack(liber::netapp::SerialStream& dtor);

private:

  liber::rsync::JobDescriptor descriptor_;
};


class SegmentInstruction : public Instruction {
public:

  static const ui32 Type = 1;

  SegmentInstruction( InstructionRaw* instruction_ptr = NULL ) : Instruction( Type, instruction_ptr ){}
  SegmentInstruction(liber::rsync::Segment::ID id) : Instruction( Type, NULL ) {
    instruction_ptr_ = new InstructionRaw( Type, sizeof( id ) );
    Segment::ID*  data_ptr = (Segment::ID*)instruction_ptr_->payload_ptr();
    *data_ptr = id;
  }
  // ~SegmentInstruction();

  // liber::rsync::Segment::ID getSegmentId() const;
  liber::rsync::Segment::ID segmentId() const {
    Segment::ID segment_id = -1;
    if ( instruction_ptr_ ) {
      Segment::ID* segment_id_ptr = (Segment::ID*)instruction_ptr_->payload_ptr();
      segment_id = *segment_id_ptr;
    }
    return segment_id;
  }
  // liber::rsync::Segment::ID segment_id_;

// protected:

//    void pack(liber::netapp::SerialStream& ctor) const;
//    void pack(liber::netapp::SerialStream& ctor);
//    bool unpack(liber::netapp::SerialStream& dtor);

};


class ChunkInstruction : public Instruction {
public:

  static const ui32 Type = 2;

  ChunkInstruction( InstructionRaw* instruction_ptr = NULL ) : Instruction( Type, instruction_ptr ){}
  ChunkInstruction( ui32 length ) : Instruction( Type, NULL ) {
    instruction_ptr_ = new InstructionRaw( Type, length );
  }
  // ~ChunkInstruction();

  ui8* const data() {
    if ( instruction_ptr_ ) {
      return (ui8* const)instruction_ptr_->payload_ptr();
    } else {
      return NULL;
    }
  }
  ui32 size() const {
    if ( instruction_ptr_ ) {
      return instruction_ptr_->payload_length();
    } else {
      return 0;
    }
  }

  // ui8* chunk_data_ptr_;
  // ui32 chunk_size_bytes_;

// protected:

//   void pack(liber::netapp::SerialStream& ctor) const;
//   void pack(liber::netapp::SerialStream& ctor);
//   bool unpack(liber::netapp::SerialStream& dtor);

};


class EndInstruction : public Instruction {
public:

  static const ui32 Type = 3;

  struct __attribute__((packed)) data {
    bool canceled;
    ui32 cancel_error;
  };

  EndInstruction( InstructionRaw* instruction_ptr ) : Instruction( Type, instruction_ptr ){}
  EndInstruction() : Instruction( Type, NULL ) {
    instruction_ptr_ = new InstructionRaw( Type, sizeof( data ) );
  }
  // ~EndInstruction();

  void cancel(RsyncError error) {
    if ( instruction_ptr_ ) {
      data_ptr()->canceled = true;
      data_ptr()->cancel_error = error;
    }
  }
  bool canceled() const {
    bool canceled = true;
    if ( data_ptr() ) {
      canceled = data_ptr()->canceled;
    }
    return canceled;
  }
  RsyncError error() const {
    RsyncError error = kRsyncIoError;
    if ( data_ptr() ) {
      error = (RsyncError)data_ptr()->cancel_error;
    }
    return error;
  }

  data* const data_ptr() const {
    if ( instruction_ptr_ ) {
      return (data*)instruction_ptr_->payload_ptr();
    } else {
      return NULL;
    }
  }

  // bool canceled_;

  // If cancelled, error that caused cancellation.
  // ui32 cancel_error_;

// protected:

//   void pack(liber::netapp::SerialStream& ctor) const;
//   void pack(liber::netapp::SerialStream& ctor);
//   bool unpack(liber::netapp::SerialStream& dtor);

};

} // End namespace rsync
} // End namespace liber


#endif // RSYNC_INSTRUCTION_H

