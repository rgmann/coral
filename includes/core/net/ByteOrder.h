#ifndef BYTE_ORDER_H
#define BYTE_ORDER_H

#include "BaseTypes.h"

namespace liber {
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

