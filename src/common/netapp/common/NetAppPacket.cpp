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



#include <stdio.h>
#include "NetAppPacket.h"
#include "ByteOrder.h"

using namespace coral::net;
using namespace coral::netapp;

//-----------------------------------------------------------------------------
NetAppPacket::NetAppPacket()
   : GenericPacket( sizeof(NetAppPacket::Data), 0 )
{
}

//-----------------------------------------------------------------------------
NetAppPacket::NetAppPacket( int type, ui32 length)
   : GenericPacket( sizeof(NetAppPacket::Data), length )
{
   if ( isAllocated() )
   {
      data()->type   = type;
      data()->length = length;
   }
}

//-----------------------------------------------------------------------------
bool NetAppPacket::allocate(const NetAppPacket::Data& rData)
{
   bool success = GenericPacket::allocate( rData.length );

   if ( success )
   {
      data()->type   = rData.type;
      data()->length = rData.length;
   }

   return success;
}

//-----------------------------------------------------------------------------
NetAppPacket::Data* const NetAppPacket::data()
{
   Data* header_ptr = NULL;

   if ( isAllocated() )
   {
      header_ptr = reinterpret_cast<Data*>(basePtr());
   }

   return header_ptr;
}

//-----------------------------------------------------------------------------
void NetAppPacket::swap(void* data_ptr, ui32 nSizeBytes)
{
   Data* header_ptr = NULL;

   if ( data_ptr && ( nSizeBytes >= sizeof( Data ) ) )
   {
      header_ptr = reinterpret_cast<Data*>( data_ptr );

      header_ptr->type   = ByteOrder::NetSwap( header_ptr->type );
      header_ptr->length = ByteOrder::NetSwap( header_ptr->length );
   }
}

