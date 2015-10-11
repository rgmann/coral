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



#ifndef GENERIC_PACKET_H
#define GENERIC_PACKET_H

#include "BaseTypes.h"

namespace liber {
namespace netapp {

class GenericPacket {
public:
   
   ///
   /// Constructor
   ///
   /// @param  base_size_bytes  Base segment size (i.e. minimum allocation size) in bytes
   /// @param  data_size_bytes  Data segment size in bytes
   ///
   GenericPacket( ui32 base_size_bytes, ui32 data_size_bytes = 0 );
   
   ///
   /// Destructor
   ///
   virtual ~GenericPacket();
   
   ///
   /// Allocate the packet to the specified size in bytes. If the packet is
   /// currently allocated, it will first be deallocated. If the specified
   /// size is smaller than the base segment size, no action is taken.
   ///
   /// @param  size_bytes  Total size of packet in bytes (base + data)
   /// @return bool        True if allocation succeeded; false otherwise.
   ///
   virtual bool allocate( ui32 size_bytes );

   ///
   /// Allocate the packet from an existing buffer. If the packet is
   /// currently allocated, it will first be deallocated. If the buffer is
   /// smaller than the base segment size, no action is taken.
   ///
   /// @param  data_ptr   Pointer to existing buffer
   /// @param  size_bytes Size of existing buffer
   /// @return bool       True if allocation succeeded; false otherwise.
   ///
   virtual bool allocate( const void* data_ptr, ui32 size_bytes );
   
   ///
   /// Deallocate the buffer back this packet. If the packet is not currently
   /// allocated, no action is taken.
   ///
   void deallocate();
   
   ///
   /// Test chether the packet is currently allocated.
   ///
   /// @return bool  True if the packet is allocated; false otherwise.
   ///
   bool isAllocated() const;
   
   ///
   /// Get size of the base segment.
   ///
   /// @return ui32  Size of base segment in bytes
   ///
   ui32 baseSize() const;
         
   ///
   /// Returns the number of bytes currently allocated to this packet (base and
   /// data).
   ///
   /// @return  ui32  Bytes currently allocated to this packet
   ///
   ui32 allocatedSize() const;
   
   ///
   /// Returns the number of bytes currently allocated to this packet's data
   /// segment.
   ///
   /// @return ui32  Bytes currently allocated to this packet's data segment
   ///
   virtual ui32 dataSize() const;
   
   ///
   /// If the packet is allocated, returns a pointer to the packet's base
   /// segment. Returns NULL if the packet is not allocated.
   ///
   /// @return void*  Pointer to the packet's base segment
   ///
   void* basePtr();

   ///
   /// If the packet is allocated, returns a pointer to the packet's base
   /// segment. Returns NULL if the packet is not allocated.
   ///
   /// @return void*  Immutable pointer to the packet's base segment
   ///
   void* const basePtr() const;
   
   ///
   /// If the packet is allocated, returns a pointer to the packet's data
   /// segment. Returns NULL if the packet is not allocated.
   ///
   /// @return void*  Pointer to the packet's data segment
   ///
   virtual void* dataPtr();

   ///
   /// If the packet is allocated, returns a pointer to the packet's data
   /// segment. Returns NULL if the packet is not allocated.
   ///
   /// @return void*  Immutable pointer to the packet's data segment
   ///
   virtual void* const  dataPtr() const;

   ///
   /// Swap the byte order of each field in the packet in place. No action is
   /// taken unless swap() is implemented.
   ///
   void swapByteOrder();


protected:

   ///
   ///
   ///
   bool allocate();

   ///
   /// Swap the byte order of the packet buffer in place. The base
   /// implementation takes no action.
   ///
   virtual void swap( void* data_ptr, ui32 size_bytes ) {};


private:

   ///
   /// GenericPacket does not have a public default constructor because the
   /// the correct base-segment size must be set during construction.
   ///
   GenericPacket();


private:

   // Pointer to beginning of the packet's base segment. The base segment
   // and data segment are allocated contiguously.
   ui8* packet_base_ptr_;

   // Size of the packet's base segment in bytes
   ui32 base_size_bytes_;

   // Total size of the packet in bytes
   ui32 allocated_size_bytes_;

};

}}

#endif // GENERIC_PACKET_H
