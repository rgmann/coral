#include <stdlib.h>
#include <stdio.h>
#include "Log.h"
#include "CircularBuffer.h"

using namespace liber;

//------------------------------------------------------------------------------
CircularBuffer::CircularBuffer()
: buffer_ptr_   ( NULL )
, head_position_( 1 )
, tail_position_( head_position_ )
, capacity_     ( 0 )
{
}

//------------------------------------------------------------------------------
CircularBuffer::CircularBuffer( const CircularBuffer& other )
{
  *this = other;
}


//------------------------------------------------------------------------------
CircularBuffer::~CircularBuffer()
{
  allocate( 0 );
}

//------------------------------------------------------------------------------
unsigned int CircularBuffer::write(
  const void* void_data_ptr,
  unsigned int byte_count
)
{
  unsigned int total_bytes_written = 0;

  const char* data_ptr = reinterpret_cast<const char*>( void_data_ptr );

  if ( buffer_ptr_ != NULL )
  {
    while ( !isFull() && ( total_bytes_written < byte_count ) )
    {
      unsigned int bytes_to_write = linearFreeSize();

      if ( bytes_to_write > ( byte_count - total_bytes_written ) )
      {
        bytes_to_write = (byte_count - total_bytes_written);
      }

      memcpy(
        &buffer_ptr_[ head_position_ ],
        &data_ptr[ total_bytes_written ],
        bytes_to_write
      );

      add( head_position_, bytes_to_write );
 
      total_bytes_written += bytes_to_write;
    }
  }
   
  return total_bytes_written;
}

//------------------------------------------------------------------------------
unsigned int CircularBuffer::write(
  std::istream& stream,
  unsigned int byte_count
)
{
  unsigned int total_bytes_written = 0;

  if ( buffer_ptr_ != NULL )
  {
    while ( !isFull() && ( total_bytes_written < byte_count ) )
    {
      unsigned int bytes_to_write = linearFreeSize();

      if ( bytes_to_write > ( byte_count - total_bytes_written ) )
      {
        bytes_to_write = (byte_count - total_bytes_written );
      }

      stream.read( &buffer_ptr_[head_position_], bytes_to_write );
      size_t bytes_read = stream.gcount();

      add( head_position_, bytes_read );
      total_bytes_written += bytes_read;

      if ( stream.eof() )
      {
        break;
      }
      if (stream.fail())
      {
        log::error("write: stream read failure - attempted %u, atual %u\n",
        total_bytes_written, bytes_read);
      }
    }
  }

  return total_bytes_written;
}

//------------------------------------------------------------------------------
unsigned int CircularBuffer::read(
  void* void_data_ptr,
  unsigned int byte_count_limit
)
{
  unsigned int total_bytes_read = 0;

  char* data_ptr = reinterpret_cast<char*>( void_data_ptr );

  if ( buffer_ptr_ != NULL )
  {
    while ( !isEmpty() && ( total_bytes_read < byte_count_limit ) )
    {
      unsigned int bytes_to_read = linearUsedSize( tail_position_ );

      if ( bytes_to_read == 0 ) break;

      if ( bytes_to_read > ( byte_count_limit - total_bytes_read ) )
      {
        bytes_to_read = ( byte_count_limit - total_bytes_read );
      }

      memcpy(
        &data_ptr[total_bytes_read],
        &buffer_ptr_[tail_position_],
        bytes_to_read
      );

      add( tail_position_, bytes_to_read );

      total_bytes_read += bytes_to_read;
    }
  }

  return total_bytes_read;
}

//------------------------------------------------------------------------------
unsigned int CircularBuffer::peek(
  void*         void_data_ptr,
  unsigned int  byte_count_limit
) const
{
  unsigned int total_bytes_read = 0;

  char* data_ptr = reinterpret_cast<char*>( void_data_ptr );

  if ( buffer_ptr_ != NULL )
  {
    unsigned int peek_position = tail_position_;

    while ( total_bytes_read < byte_count_limit )
    {
      unsigned int bytes_to_read = linearUsedSize( peek_position );

      if ( bytes_to_read == 0 ) break;

      if ( bytes_to_read > ( byte_count_limit - total_bytes_read ) )
      {
        bytes_to_read = ( byte_count_limit - total_bytes_read );
      }

      memcpy(
        &data_ptr[ total_bytes_read ],
        &buffer_ptr_[ peek_position ],
        bytes_to_read
      );

      add( peek_position, bytes_to_read );

      total_bytes_read += bytes_to_read;
    }
  }

  return total_bytes_read;
}

//------------------------------------------------------------------------------
unsigned int CircularBuffer::capacity() const
{
  return capacity_;
}

//------------------------------------------------------------------------------
void CircularBuffer::clear()
{
  head_position_ = tail_position_ = 1;
}

//------------------------------------------------------------------------------
CircularBuffer& CircularBuffer::operator=( const CircularBuffer& other )
{
  allocate( other.capacity_ );

  // Once allocated, the head and tail positions will be reset, but they should
  // actually be set to the same positions as in the source instance.
  head_position_ = other.head_position_;
  tail_position_ = other.tail_position_;

  return *this;
}

//------------------------------------------------------------------------------
bool CircularBuffer::allocate( unsigned int capacity )
{
  if ( buffer_ptr_ != NULL )
  {
    delete[] buffer_ptr_;
    buffer_ptr_ = NULL;
  }

  clear();

  capacity_ = capacity;

  if ( capacity > 0 )
  {
    buffer_ptr_ = new char[capacity_ + 1];
  }

  return ( buffer_ptr_ != NULL );
}

//------------------------------------------------------------------------------
unsigned int CircularBuffer::size() const
{
  unsigned int bytes_used = head_position_;

  if ( head_position_ < tail_position_ )
  {
    bytes_used += allocatedSize();
  }

  bytes_used -= tail_position_;

  return bytes_used;
}

//------------------------------------------------------------------------------
unsigned int CircularBuffer::allocatedSize() const
{
  return capacity_ + 1;
}

//------------------------------------------------------------------------------
unsigned int CircularBuffer::freeSize() const
{
  return capacity() - size();
}

//------------------------------------------------------------------------------
unsigned int CircularBuffer::linearFreeSize() const
{
  unsigned int writable_bytes = 0;

  if ( tail_position_ <= head_position_ )
  {
    writable_bytes = allocatedSize() - head_position_;
    if ( tail_position_ == 0 ) writable_bytes--;
  }
  else
  {
    writable_bytes = ( tail_position_ - head_position_ ) - 1;
  }

  return writable_bytes;
}

//------------------------------------------------------------------------------
unsigned int CircularBuffer::linearUsedSize(
  unsigned int read_position
) const
{
  int readable_bytes = (int)head_position_ - (int)read_position;

  if ( readable_bytes < 0 )
  {
    readable_bytes = allocatedSize() - read_position;
  }

  return readable_bytes;
}

//----------------------------------------------------------------------------
void CircularBuffer::add(
  unsigned int& position,
  unsigned int  move_count
) const
{
  position += move_count;
  position %= allocatedSize();
}

//------------------------------------------------------------------------------
bool CircularBuffer::isFull() const
{
  return ( ( ( head_position_ + 1 ) % allocatedSize() ) == tail_position_ );
}

//------------------------------------------------------------------------------
bool CircularBuffer::isEmpty() const
{
  return ( head_position_ == tail_position_ );
}

