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


#ifndef BYTE_ORDER_H
#define BYTE_ORDER_H

#include "BaseTypes.h"

namespace coral {
namespace net {

namespace ByteOrder {

  enum Endianness {
    NetworkOrder = 0,
    BigEndian    = 0,

    LittleEndian = 1
  };


  void NetSwapInPlace(i16&);
  void NetSwapInPlace(ui16&);
  void NetSwapInPlace(i32&);
  void NetSwapInPlace(ui32&);
  void NetSwapInPlace(i64&);
  void NetSwapInPlace(ui64&);

  i16  NetSwap(i16);
  ui16 NetSwap(ui16);
  i32  NetSwap(i32);
  ui32 NetSwap(ui32);
  i64  NetSwap(i64);
  ui64 NetSwap(ui64);


  void Swap(i16&);
  void Swap(ui16&);
  void Swap(i32&);
  void Swap(ui32&);
  void Swap(i64&);
  void Swap(ui64&);

  Endianness platformByteOrder();

}

}}

#endif // BYTE_ORDER_H

