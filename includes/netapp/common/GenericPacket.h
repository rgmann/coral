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
   
   enum Type
   {
      TypeNotSet
   };
   
   struct __attribute__((__packed__)) Data
   {
   };
   
   GenericPacket();
   
   explicit GenericPacket(const GenericPacket &other);
   
   explicit GenericPacket(ui32 nSizeBytes);
   GenericPacket(ui32 nDataSizeBytes, ui32 nBodySizeBytes);
   
   virtual ~GenericPacket();
   
   bool  allocate();
   virtual bool  allocate(ui32 sizeBytes);
   virtual bool  allocate(ui32 nDataSizeBytes, ui32 nBodySizeBytes);
   
   void  deallocate();
   
   bool  isAllocated() const;
   
   void  destroy();
         
   /**
    * Returns the the full number of bytes allocated to this packet.
    * As this is a property of the base packet, the method cannot be overriden.
    */
   ui32 allocatedSize() const;
   
   /**
    * This size of the immediate packet's Data definition.
    * All Packet instances must override this method.
    */
   virtual ui32 dataSize() const;
   
   virtual bool  pack(void** pData, ui32 &nSizeBytes) const;
   
   virtual bool  unpack(const void* pData, ui32 nSizeBytes);
   
   GenericPacket& operator= (const GenericPacket& other);
   
   /**
    * Returns a pointer to the very beginning of the packet.  Cannot be
    * overriden.
    */
   void*       basePtr();
   void* const basePtr() const;
   
   virtual void*        dataPtr();
   virtual void* const  dataPtr() const;

   virtual void swapByteOrder();
   virtual void swap(void* pData, ui32 nSizeBytes) = 0;

protected:
   
   unsigned char* m_pPkt;

   ui32   mnDataSizeBytes;      
   ui32   m_nSizeBytes;
};

}}

#endif // GENERIC_PACKET_H
