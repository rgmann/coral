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


  void NetSwap(i16&);
  void NetSwap(ui16&);
  void NetSwap(i32&);
  void NetSwap(ui32&);
  void NetSwap(i64&);
  void NetSwap(ui64&);

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

