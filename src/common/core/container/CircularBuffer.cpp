// 
// Copyright (c) 2015, Robert Glissmann
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// 
// * Redistributions of source code must retain the above copyright notice, this
// list of conditions and the following disclaimer.
// 
// * Redistributions in binary form must reproduce the above copyright notice,
// this list of conditions and the following disclaimer in the documentation
// and/or other materials provided with the distribution.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// 

// %% license-end-token %%
// 
// Author: Robert.Glissmann@gmail.com (Robert Glissmann)
// 
// 



#include <stdlib.h>
#include <stdio.h>
#include "Log.h"
#include "CircularBuffer.h"

using namespace coral;

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
size_t CircularBuffer::write( const void* void_data_ptr, size_t byte_count )
{
   size_t total_bytes_written = 0;

   const char* data_ptr = reinterpret_cast<const char*>( void_data_ptr );

   if ( buffer_ptr_ != NULL )
   {
      while ( !isFull() && ( total_bytes_written < byte_count ) )
      {
         size_t bytes_to_write = linearFreeSize();

         if ( bytes_to_write > ( byte_count - total_bytes_written ) )
         {
            bytes_to_write = ( byte_count - total_bytes_written );
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
size_t CircularBuffer::write( std::istream& stream, size_t byte_count )
{
  size_t total_bytes_written = 0;

  if ( buffer_ptr_ != NULL )
  {
    while ( !isFull() && ( total_bytes_written < byte_count ) )
    {
      size_t bytes_to_write = linearFreeSize();

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
size_t CircularBuffer::read( void* void_data_ptr, size_t byte_count_limit )
{
   size_t total_bytes_read = 0;

   char* data_ptr = reinterpret_cast<char*>( void_data_ptr );

   if ( buffer_ptr_ != NULL )
   {
      while ( !isEmpty() && ( total_bytes_read < byte_count_limit ) )
      {
         size_t bytes_to_read = linearUsedSize( tail_position_ );

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
size_t CircularBuffer::peek( void* void_data_ptr, size_t byte_count_limit ) const
{
   size_t total_bytes_read = 0;

   char* data_ptr = reinterpret_cast<char*>( void_data_ptr );

   if ( buffer_ptr_ != NULL )
   {
      size_t peek_position = tail_position_;

      while ( total_bytes_read < byte_count_limit )
      {
         size_t bytes_to_read = linearUsedSize( peek_position );

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
size_t CircularBuffer::capacity() const
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
   if ( this != &other )
   {
      allocate( other.capacity_ );

      // Once allocated, the head and tail positions will be reset, but they should
      // actually be set to the same positions as in the source instance.
      head_position_ = other.head_position_;
      tail_position_ = other.tail_position_;
   }

   return *this;
}

//------------------------------------------------------------------------------
bool CircularBuffer::allocate( size_t capacity )
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
size_t CircularBuffer::size() const
{
   size_t bytes_used = head_position_;

   if ( head_position_ < tail_position_ )
   {
      bytes_used += allocatedSize();
   }

   bytes_used -= tail_position_;

   return bytes_used;
}

//------------------------------------------------------------------------------
size_t CircularBuffer::allocatedSize() const
{
   return capacity_ + 1;
}

//------------------------------------------------------------------------------
size_t CircularBuffer::freeSize() const
{
   return capacity() - size();
}

//------------------------------------------------------------------------------
size_t CircularBuffer::linearFreeSize() const
{
   size_t writable_bytes = 0;

   if ( tail_position_ <= head_position_ )
   {
      writable_bytes = allocatedSize() - head_position_;

      if ( tail_position_ == 0 )
      {
         writable_bytes--;
      }
   }
   else
   {
      writable_bytes = ( tail_position_ - head_position_ ) - 1;
   }

   return writable_bytes;
}

//------------------------------------------------------------------------------
size_t CircularBuffer::linearUsedSize( size_t read_position ) const
{
   int readable_bytes = (int)head_position_ - (int)read_position;

   if ( readable_bytes < 0 )
   {
      readable_bytes = allocatedSize() - read_position;
   }

   return readable_bytes;
}

//----------------------------------------------------------------------------
void CircularBuffer::add( size_t& position, size_t  move_count ) const
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

