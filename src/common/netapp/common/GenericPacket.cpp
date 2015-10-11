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
#include <string.h>
#include "Log.h"
#include "GenericPacket.h"

using namespace liber::netapp;

//------------------------------------------------------------------------------
GenericPacket::GenericPacket()
   : packet_base_ptr_(NULL)
   , base_size_bytes_(0)
   , allocated_size_bytes_(0)
{
}

//------------------------------------------------------------------------------
GenericPacket::GenericPacket( ui32 base_size_bytes, ui32 data_size_bytes )
   : packet_base_ptr_( NULL )
   , base_size_bytes_( base_size_bytes )
   , allocated_size_bytes_( base_size_bytes + data_size_bytes )
{
   allocate();
}

//------------------------------------------------------------------------------
GenericPacket::~GenericPacket()
{
   deallocate();
}

//------------------------------------------------------------------------------
bool GenericPacket::allocate()
{
   bool success = false;

   // This overload of allocate expects the allocated size to be precomputed,
   // so if it is zero, something is wrong.
   if ( allocated_size_bytes_ != 0 )
   {
      // Deallocate the buffer if it is currently allocated.
      if ( packet_base_ptr_ != NULL )
      {
         delete[] packet_base_ptr_;
         packet_base_ptr_ = NULL;
      }

      packet_base_ptr_ = new (std::nothrow) ui8[ allocated_size_bytes_ ];

      if ( ( success = ( packet_base_ptr_ != NULL ) ) )
      {
         memset( packet_base_ptr_, 0, allocated_size_bytes_ );
      }
      else
      {
         log::error(
            "GenericPacket::allocate: "
            "Failed to allocate buffer of size %u.\n",
            allocated_size_bytes_
         );
      }
   }
   else
   {
      log::debug( "GenericPacket::allocate: Already allocated!\n" );
   }
   
   return success;
}

//------------------------------------------------------------------------------
bool GenericPacket::allocate( ui32 size_bytes )
{
   bool success = false;

   if ( ( size_bytes > 0 ) && ( size_bytes >= base_size_bytes_ ) )
   {
      allocated_size_bytes_ = size_bytes;
      success = allocate();
   }

   return success;
}

//------------------------------------------------------------------------------
bool GenericPacket::isAllocated() const
{
   return ( packet_base_ptr_ != NULL );
}

//------------------------------------------------------------------------------
void GenericPacket::deallocate()
{
   if ( packet_base_ptr_ != NULL )
   {
      delete[] packet_base_ptr_;
      packet_base_ptr_ = NULL;
   }
   
   allocated_size_bytes_ = 0;

   // Note:
   // Do not clear the base_size_bytes_ member as that is an attribute
   // of the type rather than the instance and must always be set.
}

//------------------------------------------------------------------------------
ui32 GenericPacket::baseSize() const
{
   return base_size_bytes_;
}

//------------------------------------------------------------------------------
ui32 GenericPacket::allocatedSize() const
{
   return allocated_size_bytes_;
}

//------------------------------------------------------------------------------
ui32 GenericPacket::dataSize() const
{
   ui32 data_size_bytes = allocated_size_bytes_;

   if ( isAllocated() )
   {
      data_size_bytes -= base_size_bytes_;
   }

   return data_size_bytes;
}

//------------------------------------------------------------------------------
void* GenericPacket::basePtr()
{
   return reinterpret_cast<void*>(packet_base_ptr_);
}

//------------------------------------------------------------------------------
void* const GenericPacket::basePtr() const
{
   return reinterpret_cast<void* const>(packet_base_ptr_);
}

//------------------------------------------------------------------------------
void* GenericPacket::dataPtr()
{
   ui8* data_ptr = NULL;

   if ( packet_base_ptr_ != NULL )
   {
      data_ptr = reinterpret_cast<ui8*>( basePtr() ) + base_size_bytes_;
   }
   
   return data_ptr;
}

//------------------------------------------------------------------------------
void* const GenericPacket::dataPtr() const
{
   ui8* data_ptr = NULL;
   
   if ( packet_base_ptr_ != NULL )
   {
      data_ptr = reinterpret_cast<ui8*>( basePtr() ) + base_size_bytes_;
   }

   return data_ptr;
}

//------------------------------------------------------------------------------
bool GenericPacket::allocate( const void* packet_ptr, ui32 size_bytes )
{
   bool success = false;

   if ( packet_ptr != NULL )
   {
      if ( size_bytes >= base_size_bytes_ )
      {
         // Begin by destroying the packet if it is already allocated.
         deallocate();

         // The header size is known since it is supplied...
         if ( allocate( size_bytes ) )
         {
            // Copy the packet.
            memcpy( packet_base_ptr_, packet_ptr, allocatedSize() );
            success = true;
         }
         else
         {
            log::error("GenericPacket::unpack: Allocation failed\n");
         }
      }
      else
      {
         log::error("GenericPacket::unpack - "
                    "Buffer smaller than minimum data size.\n");
      }
   }
   else
   {
      log::error("GenericPacket::unpack: NULL\n");
   }

   return success;
}

//-----------------------------------------------------------------------------
void GenericPacket::swapByteOrder()
{
   if ( isAllocated() )
   {
      swap( packet_base_ptr_, allocatedSize() );
   }
}

