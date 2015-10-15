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
#include <arpa/inet.h>
#include "ByteOrder.h"

using namespace coral::net;

#define NET_SWAP_IP(type) \
void ByteOrder::NetSwapInPlace(type& val) \
{ \
  if (ByteOrder::platformByteOrder() == ByteOrder::LittleEndian) \
  { \
    ByteOrder::Swap(val); \
  } \
} 

#define NET_SWAP(type) \
type ByteOrder::NetSwap(type val) \
{ \
  type swapped = val; \
  if (ByteOrder::platformByteOrder() == ByteOrder::LittleEndian) \
  { \
    ByteOrder::Swap(swapped); \
  } \
  return swapped; \
} 


//-----------------------------------------------------------------------------
NET_SWAP_IP(i16)
NET_SWAP_IP(ui16)
NET_SWAP_IP(i32)
NET_SWAP_IP(ui32)
NET_SWAP_IP(i64)
NET_SWAP_IP(ui64)

//-----------------------------------------------------------------------------
NET_SWAP(i16)
NET_SWAP(ui16)
NET_SWAP(i32)
NET_SWAP(ui32)
NET_SWAP(i64)
NET_SWAP(ui64)

//-----------------------------------------------------------------------------
void ByteOrder::Swap(ui16& val)
{
  ui8* lpU = reinterpret_cast<ui8*>(&val);
  ui8* lpL = lpU + 1;

  *lpU = *lpU ^ *lpL;
  *lpL = *lpU ^ *lpL;
  *lpU = *lpU ^ *lpL;
}

//-----------------------------------------------------------------------------
void ByteOrder::Swap(i16& val)
{
  ui16* lpVal = reinterpret_cast<ui16*>(&val);
  Swap((ui16&)*lpVal);
}

//-----------------------------------------------------------------------------
void ByteOrder::Swap(ui32& val)
{
  ui16* lpU = reinterpret_cast<ui16*>(&val);
  ui16* lpL = lpU + 1;

  Swap((ui16&)*lpU);
  Swap((ui16&)*lpL);

  *lpU = *lpU ^ *lpL;
  *lpL = *lpU ^ *lpL;
  *lpU = *lpU ^ *lpL;
}

//-----------------------------------------------------------------------------
void ByteOrder::Swap(i32& val)
{
  ui32* lpVal = reinterpret_cast<ui32*>(&val);
  Swap((ui32&)*lpVal);
}

//-----------------------------------------------------------------------------
void ByteOrder::Swap(ui64& val)
{
  ui32* lpU = reinterpret_cast<ui32*>(&val);
  ui32* lpL = lpU + 1;

  Swap((ui32&)*lpU);
  Swap((ui32&)*lpL);

  *lpU = *lpU ^ *lpL;
  *lpL = *lpU ^ *lpL;
  *lpU = *lpU ^ *lpL;
}

//-----------------------------------------------------------------------------
void ByteOrder::Swap(i64& val)
{
  ui64* lpVal = reinterpret_cast<ui64*>(&val);
  Swap((ui64&)*lpVal);
}

//-----------------------------------------------------------------------------
ByteOrder::Endianness ByteOrder::platformByteOrder()
{
  Endianness lEnd = ByteOrder::BigEndian;

  static ui16 val = 0x1234;
  if (reinterpret_cast<char*>(&val)[0] == 0x34)
  {
    lEnd = ByteOrder::LittleEndian;
  }

  return lEnd;
}

