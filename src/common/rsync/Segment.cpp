#include <string.h>
#include <stdio.h>
#include <sstream>
#include "Log.h"
#include "Segment.h"
#include "CircularBuffer.h"

using namespace liber;
using namespace liber::rsync;
using namespace liber::netapp;

struct PackedSegment {
  Segment::ID id;
  ui32        size;
  ui32        offset;
  i32         weak;
  Hash128     strong;
};


static const ui32 M = 0x00010000;

void PrintSegRsync(unsigned char* pBlock, int nBytes)
{
   for (int ind = 0; ind < nBytes; ind++)
      printf("%d ", *(pBlock + ind));
   printf("\n");
}

//------------------------------------------------------------------------------
Segment::Segment()
: Serializable()
, segment_id_           ( Segment::EndOfStream )
, data_ptr_             ( NULL )
, segment_size_         ( 0 )
, offset_               ( 0 )
, virtual_segment_size_ ( 0 )
{
}

//------------------------------------------------------------------------------
Segment::Segment(ID id, ui32 offset)
: Serializable()
, segment_id_           ( id )
, data_ptr_             ( NULL )
, segment_size_         ( 0 )
, offset_               ( offset )
, virtual_segment_size_ ( 0 )
{
}

//------------------------------------------------------------------------------
Segment::Segment( ui32 virtual_segment_size )
: Serializable()
, segment_id_           ( Segment::EndOfStream )
, data_ptr_             ( NULL )
, segment_size_         ( 0 )
, offset_               ( 0 )
, virtual_segment_size_ ( 0 )
{
  resize( virtual_segment_size );
}

//------------------------------------------------------------------------------
Segment::Segment( const Segment& other )
: Serializable()
, segment_id_           ( other.segment_id_ )
, weak_checksum_        ( other.weak_checksum_ )
, data_ptr_             ( NULL )
, segment_size_         ( other.segment_size_ )
, offset_               ( other.offset_ )
, virtual_segment_size_ ( other.virtual_segment_size_ )
{
  // segment_id_ = other.segment_id_;
  // segment_size_ = other.segment_size_;
  // virtual_segment_size_ = other.virtual_segment_size_;
  // offset_ = other.offset_;
  // weak_checksum_ = other.weak_checksum_;
  if ( other.data_ptr_ )
  {
    data_ptr_ = new ui8[ virtual_segment_size_ ];
    memcpy( data_ptr_, other.data_ptr_, virtual_segment_size_ );
  }
}

//------------------------------------------------------------------------------
Segment::~Segment()
{
  if ( data_ptr_ )
  {
    delete[] data_ptr_;
    data_ptr_ = NULL;
  }
}

//------------------------------------------------------------------------------
void Segment::setData(
  const ui8* data_ptr,
  ui32  virtual_segment_size,
  ui32  segment_size,
  const Adler32Checksum* previous_weak_checksum_ptr
)
{
  // Invalidate the checksum since the segment will contain different data.
  strong_checksum_.invalidate();

  // Resize the segment if necessary.
  resize( virtual_segment_size );

  segment_size_ = segment_size;

  memcpy( data_ptr_, data_ptr, segment_size_ );

  computeWeak( previous_weak_checksum_ptr );
}

//------------------------------------------------------------------------------
void Segment::setData(
  CircularBuffer& buffer,
  ui32 virtual_segment_size,
  const Adler32Checksum* previous_weak_checksum_ptr)
{
  strong_checksum_.invalidate();

  // Resize the segment if necessary.
  resize( virtual_segment_size );

  segment_size_ = buffer.peek((char*)data_ptr_, virtual_segment_size );

  computeWeak( previous_weak_checksum_ptr );
}

//------------------------------------------------------------------------------
void Segment::setData(
  std::istream& stream,
  ui32 virtual_segment_size,
  const Adler32Checksum* previous_weak_checksum_ptr
)
{
  strong_checksum_.invalidate();

  // Resize the segment if necessary.
  resize( virtual_segment_size );

  stream.read((char*)data_ptr_, virtual_segment_size );
  segment_size_ = stream.gcount();

  computeWeak( previous_weak_checksum_ptr );
}

//------------------------------------------------------------------------------
const Adler32Checksum& Segment::getWeak()
{
  return weak_checksum_;
}

//------------------------------------------------------------------------------
Md5Hash& Segment::getStrong()
{
  // If the strong checksum has not been computed, and the data is set,
  // compute it now.
  if ( data_ptr_ && !strong_checksum_.isValid() )
  {
    strong_checksum_.hashify( data_ptr_, segment_size_ );
  }

  return strong_checksum_;
}

//------------------------------------------------------------------------------
Md5Hash& Segment::getStrong( std::ifstream& istream )
{
  Segment::data(istream);
  return Segment::getStrong();
}

//------------------------------------------------------------------------------
ui8* const Segment::data()
{
   return data_ptr_;
}

//------------------------------------------------------------------------------
ui8* const Segment::data(std::ifstream &istream)
{
  if ( data_ptr_ )
  {
    delete[] data_ptr_;
    data_ptr_ = NULL;
  }
  data_ptr_ = new ui8[segment_size_];

  istream.seekg(offset_, istream.beg);
  istream.read((char*)data_ptr_, segment_size_);

  return Segment::data();
}

//------------------------------------------------------------------------------
ui32 Segment::size() const
{
  return segment_size_;
}

//------------------------------------------------------------------------------
Segment::ID Segment::getID() const
{
  return segment_id_;
}

//------------------------------------------------------------------------------
bool Segment::endOfStream() const
{
  return (getID() == -1);
}

//------------------------------------------------------------------------------
void Segment::release()
{
  data_ptr_ = NULL;
  segment_size_ = 0;
}

//------------------------------------------------------------------------------
bool Segment::resize( ui32 virtual_segment_size )
{
  bool resize_success = false;

  if ( virtual_segment_size > 0 )
  {
    // If the data buffer is already allocated, but the size is different,
    // deallocate the current buffer.
    if ( data_ptr_ && ( virtual_segment_size_ != virtual_segment_size ) )
    {
      delete[] data_ptr_;
      data_ptr_ = NULL;
    }

    // Save the new allocated segment size.
    virtual_segment_size_ = virtual_segment_size;

    // If the buffer is not allocated, allocate it with the requested size.
    if ( data_ptr_ == NULL )
    {
      data_ptr_ = new ui8[ virtual_segment_size_ ];
    }

    resize_success = true;
  }

  return resize_success;
}

//------------------------------------------------------------------------------
void Segment::computeWeak( const Adler32Checksum* previous_weak_checksum_ptr )
{
  if ( previous_weak_checksum_ptr )
  {
    rollWeak(
      weak_checksum_,
      offset_,
      virtual_segment_size_,
      *previous_weak_checksum_ptr
    );
  }
  else
  {
    computeWeak( weak_checksum_, offset_, virtual_segment_size_ );
  }
}

//------------------------------------------------------------------------------
ui8 Segment::getByte( ui32 offset, OffsetBase base ) const
{
  ui8 byte = 0;

  if ( base == StreamStart )
  {
    offset = ( offset > offset_ ) ? ( offset - offset_ ) : 0;
  }

  if ( data_ptr_ && ( offset < size() ) )
  {
    byte = data_ptr_[ offset ];
  }

  return byte;
}

//------------------------------------------------------------------------------
bool Segment::isValid() const
{
  bool lbValid = true;
  lbValid &= (segment_id_ >= 0);
  lbValid &= (segment_size_ > 0);
  return lbValid;
}

//------------------------------------------------------------------------------
void Segment::pack(SerialStream& ctor) const
{
//  ctor.write((ui32)segment_id_);
//  ctor.write(size());
//  ctor.write(offset_);
//  ctor.write((ui32)weak_checksum_.checksum());

//  Hash128 strong;
//  getStrong().get(&strong);
//  ctor.write((char*)&strong, sizeof(strong));
}

//------------------------------------------------------------------------------
void Segment::pack(SerialStream& ctor)
{
  ctor.write((ui32)segment_id_);
  ctor.write(size());
  ctor.write(offset_);
  ctor.write((ui32)weak_checksum_.checksum());

  //std::string strongBuffer;
  Hash128 strong;
  getStrong().get(&strong);
  //strongBuffer.assign((char*)&strong, sizeof(strong));
  //ctor.write(strongBuffer);
  ctor.write((char*)&strong, sizeof(strong));
}

//------------------------------------------------------------------------------
bool Segment::unpack(SerialStream& dtor)
{
  if (dtor.read(segment_id_) == false)
  {
    log::error("Segment::unpack - Failed to parse segment ID\n");
    return false;
  }

  if (dtor.read(segment_size_) == false)
  {
    log::error("Segment::unpack - Failed to parse segment size\n");
    return false;
  }

  if (dtor.read(offset_) == false)
  {
    log::error("Segment::unpack - Failed to parse segment offset\n");
    return false;
  }

  if (dtor.read(weak_checksum_.s) == false)
  {
    log::error("Segment::unpack - Failed to parse weak hash\n");
    return false;
  }

  std::string strongBuffer;
  if (dtor.read(strongBuffer) != SerialStream::ReadOk)
  {
    log::error("Segment::unpack - Failed to parse strong hash\n");
    return false;
  }
  else
  {
    Hash128 strong;
    memcpy(&strong, strongBuffer.data(), strongBuffer.size());
    strong_checksum_ = Md5Hash(strong);
  }

  return true;
}

//------------------------------------------------------------------------------
std::string Segment::debugDump()
{
  std::stringstream ss;

  ss << "Segment #" << getID()
     << " (size=" << size() 
     << ", weak=" << std::hex << getWeak().checksum() 
     << ")" << std::endl;
  if (data())
  {
    ui8* lpData = data();
    for (ui32 lnOffset = 0; lnOffset < size(); lnOffset++)
    {
      ss << " 0x" << std::hex << (ui16)lpData[lnOffset];
      if ((lnOffset > 0) && (lnOffset % 16 == 0))
      {
        ss << std::endl;
      }
    }
  }
  else
  {
    ss << "  Empty" << std::endl;
  }

  return ss.str();
}

//------------------------------------------------------------------------------
void Segment::
computeWeak(Adler32Checksum& next,
            ui32             nOffset,
            ui32             nBlockSizeBytes)
{
#ifdef DEBUG_WEAK_CSUM
  printf("ComputeWeak: offs=%u, size=%u\n", nOffset, nBlockSizeBytes);
#endif

  next.k = nOffset;
  next.l = (nOffset + nBlockSizeBytes - 1);

  next.a = 0;
  for (ui32 lnByte = 0; lnByte < nBlockSizeBytes; ++lnByte)
  {
    next.a += (int)getByte(lnByte);
  }
   
  next.b = 0;
  for (ui32 lnByte = 0; lnByte < nBlockSizeBytes; ++lnByte)
  {
    next.b += (next.l - (next.k + lnByte) + 1) * (int)getByte(lnByte);
  }


  next.s = ((next.b << 16) & 0xFFFF0000) | (next.a & 0x0000FFFF);
  next.x_k = getByte(next.k - nOffset);
  next.x_l = getByte(next.l - nOffset);


#ifdef DEBUG_WEAK_CSUM
  printf("CW(%u): k = %d, l = %d, a = %d, b = %d, s = %x, (%c,%c)\n", 
         nOffset, next.k, next.l, next.a, next.b, next.s, next.x_k, next.x_l);
#endif
}

//------------------------------------------------------------------------------
void Segment::
rollWeak(Adler32Checksum&       next,
         ui32                   nOffset,
         ui32                   nBlockSizeBytes,
         const Adler32Checksum& prev)
{
  next.k = prev.k + 1;
  next.l = prev.l + 1;

  // Transform from file offset to offset within the segment. 
  next.a = (prev.a - prev.x_k);
  next.a += (int)getByte(next.l - nOffset);

  next.b = (prev.b - ((prev.l - prev.k + 1) * prev.x_k) + next.a);

  next.s = ((next.b << 16) & 0xFFFF0000) | (next.a & 0x0000FFFF);
  next.x_k = getByte(next.k - nOffset);
  next.x_l = getByte(next.l - nOffset);


#ifdef DEBUG_WEAK_CSUM
  printf("RW(%u): k = %d, l = %d, a = %d, b = %d, s = %x, (l-k+1)*x_k=%u, (0x%02X,0x%02X)\n", 
         next.l - nOffset, next.k, next.l, next.a, next.b, next.s, (prev.l-prev.k+1)*prev.x_k, next.x_k, next.x_l);
#endif
}



//------------------------------------------------------------------------------
SegmentComparator::SegmentComparator( Segment* segment_ptr )
: segment_ptr_( segment_ptr )
{
}

//------------------------------------------------------------------------------
bool SegmentComparator::operator() ( Segment* segment_ptr )
{
  bool segments_equal = false;

  if ( segment_ptr_->getWeak().checksum() == segment_ptr->getWeak().checksum() )
  {
    if ( segment_ptr_->getStrong() == segment_ptr->getStrong() )
    {
      segments_equal = true;
    }
  }

  return segments_equal;
}



//------------------------------------------------------------------------------
void SegmentDestructor::operator() ( Segment*& segment_ptr )
{
  if ( segment_ptr )
  {
    delete segment_ptr;
    segment_ptr = NULL;
  }
}


