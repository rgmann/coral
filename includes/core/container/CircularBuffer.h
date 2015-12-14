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

namespace coral {

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

  ///
  /// Allocate the buffer to the specified size in bytes.
  ///
  /// @param capacity_bytes  Capacity in bytes
  /// @return bool  True if buffer was successfully allocated; false otherwise
  ///
  bool allocate( size_t capacity_bytes );

  ///
  /// Write a specified number of bytes from the specified memory location into
  /// the end of the circular buffer. Returns the actual number of bytes that
  /// were copied. The actual number of bytes copied may be less the specified
  /// number of bytes if there is insufficient space in the circular buffer.
  ///
  /// @param  data_ptr  Data block pointer
  /// @param  byte_count Number of bytes to write into the circular buffer
  /// @return size_t  Number of bytes successfully written
  ///
  size_t write( const void* data_ptr, size_t byte_count );

  ///
  /// Write a specified number of bytes from the stream into the end of the
  /// circular buffer. Returns the actual number of bytes that were copied.
  /// The actual number of bytes copied may be less the specified number of
  /// bytes if there is insufficient space in the circular buffer or if the
  /// stream EOF condition becomes true before the specified number of bytes
  /// are copied.
  ///
  /// @param  stream  Input stream
  /// @param  byte_count  Number of bytes to write into the circular buffer
  /// @return size_t  Number of bytes successfully written
  ///
  size_t write( std::istream& stream, size_t byte_count );

  ///
  /// Read a specified number of bytes out of the head of the circular buffer to
  /// the specified memory location. Returns the actual number of bytes that
  /// were read out of the circular buffer. The number of bytes read out of the
  /// circular buffer may be less than the specified number of bytes if there
  /// fewer bytes available.
  ///
  /// @param  data_ptr  Pointer to destination buffer
  /// @param  max_bytes Maximum number of bytes to read into the destination
  ///                   buffer
  /// @return size_t    Number of bytes read into the destination buffer
  ///
  size_t read( void* data_ptr, size_t max_bytes );

  ///
  /// Copy a specified number of bytes from the head of the circular buffer to
  /// the specified memory location without modifying the state of the buffer
  /// itself. Returns the actual number of bytes that copied. The number of
  /// bytes read out of the circular buffer may be less than the specified
  /// number of bytes if there fewer bytes available.
  ///
  /// @param  data_ptr  Pointer to destination buffer
  /// @param  max_bytes Maximum number of bytes to read into the destination
  ///                   buffer
  /// @return size_t    Number of bytes read into the destination buffer
  ///
  size_t peek( void* data_ptr, size_t max_bytes ) const;

  ///
  /// Test whether the buffer currently contains any data. The method returns
  /// true if the buffer contains no data or is unallocated. Otherwise, it
  /// returns false.
  ///
  /// @return bool  True if the buffer is empty; false otherwise
  ///
  bool isEmpty() const;

  ///
  /// Test whether the buffer is currently full (i.e. no more data may be
  /// written to the buffer). The method returns false if more data may be
  /// written to the buffer or if the buffer is unallocated. Otherwise, it
  /// returns true.
  ///
  /// @return bool  True if the buffer is full
  ///
  bool isFull() const;

  ///
  /// Get the number of bytes currently available in the buffer.
  ///
  /// @return size_t  Number of bytes currently stored in the buffer
  ///
  size_t size() const;

  ///
  /// Get the capacity of the buffer in bytes.
  ///
  /// @return size_t  Number of bytes in the buffer
  ///
  size_t capacity() const;

  ///
  /// Clear the buffer. This is an O(1) operation as the head and tail are
  /// simply set equal to each other.
  ///
  /// @return void
  ///
  void clear();

  CircularBuffer& operator= ( const CircularBuffer& );


private:
   
  void add( size_t& position, size_t move_count ) const;

  size_t freeSize() const;

  size_t linearFreeSize() const;

  size_t linearUsedSize(size_t read_position ) const;

  size_t allocatedSize() const;


private:

  // Internal data buffer allocated to store capacity_ + 1 bytes of data.
  // The buffer is empty if head_position_ == tail_position_.
  // The buffer is full if head_position_ + 1 == tail_position_.
  char* buffer_ptr_;

  // Current head position
  size_t head_position_;

  // Current tail position
  size_t tail_position_;

  // Number of bytes that may be stored in this CircularBuffer instance
  size_t capacity_;
};

} // namespace coral

#endif // CIRCULAR_BUFFER_H
