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



#ifndef  CIRCULAR_BUFFER_H
#define  CIRCULAR_BUFFER_H

#include <istream>

namespace liber {

class CircularBuffer {
public:

  //
  // Construct a circular buffer
  //
  CircularBuffer();

  //
  // Construct a CircularBuffer from another CircularBuffer instance
  //
  CircularBuffer( const CircularBuffer& );

  //
  // Destroy the CircularBuffer
  //
  ~CircularBuffer();

  //
  // Allocate the buffer to the specified size in bytes.
  //
  // Preconditions:
  // None
  //
  // Postconditions:
  // 
  //
  bool allocate( unsigned int capacity_bytes );

  //
  // Write a specified number of bytes from the specified memory location into
  // the end of the circular buffer. Returns the actual number of bytes that
  // were copied. The actual number of bytes copied may be less the specified
  // number of bytes if there is insufficient space in the circular buffer.
  //
  unsigned int write( const void* data_ptr, unsigned int byte_count );

  //
  // Write a specified number of bytes from the stream into the end of the
  // circular buffer. Returns the actual number of bytes that were copied.
  // The actual number of bytes copied may be less the specified number of
  // bytes if there is insufficient space in the circular buffer or if the
  // stream EOF condition becomes true before the specified number of bytes
  // are copied.
  //
  unsigned int write( std::istream& stream, unsigned int byte_count );

  //
  // Read a specified number of bytes out of the head of the circular buffer to
  // the specified memory location. Returns the actual number of bytes that
  // were read out of the circular buffer. The number of bytes read out of the
  // circular buffer may be less than the specified number of bytes if there
  // fewer bytes available.
  //
  unsigned int read( void* data_ptr, unsigned int byte_count_limit );

  //
  // Copy a specified number of bytes from the head of the circular buffer to
  // the specified memory location without modifying the state of the buffer
  // itself. Returns the actual number of bytes that copied. The number of
  // bytes read out of the circular buffer may be less than the specified
  // number of bytes if there fewer bytes available.
  //
  unsigned int peek( void* data_ptr, unsigned int byte_count_limit ) const;

  //
  // Test whether the buffer currently contains any data. The method returns
  // true if the buffer contains no data or is unallocated. Otherwise, it
  // returns false.
  //
  bool isEmpty() const;

  //
  // Test whether the buffer is currently full (i.e. no more data may be
  // written to the buffer). The method returns false if more data may be
  // written to the buffer or if the buffer is unallocated. Otherwise, it
  // returns true.
  //
  bool  isFull() const;

  //
  // Get the number of bytes currently available in the buffer.
  //
  unsigned int size() const;

  //
  // Get the capacity of the buffer in bytes.
  //
  unsigned int capacity() const;

  //
  // Clear the buffer. This is an O(1) operation as the head and tail are
  // simply set equal to each other.
  //
  void clear();

  //
  CircularBuffer& operator= ( const CircularBuffer& );


private:
   
  void add( unsigned int& position, unsigned int move_count ) const;

  unsigned int freeSize() const;

  unsigned int linearFreeSize() const;

  unsigned int linearUsedSize(unsigned int read_position ) const;

  unsigned int allocatedSize() const;


private:

  // Internal data buffer allocated to store capacity_ + 1 bytes of data.
  // The buffer is empty if head_position_ == tail_position_.
  // The buffer is full if head_position_ + 1 == tail_position_.
  char* buffer_ptr_;

  // Current head position
  unsigned int head_position_;

  // Current tail position
  unsigned int tail_position_;

  // Number of bytes that may be stored in this CircularBuffer instance
  unsigned int capacity_;
};

} // namespace liber

#endif // CIRCULAR_BUFFER_H
