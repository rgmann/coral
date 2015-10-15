#include "ByteOrder.h"
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string>

using namespace coral::net;

int main(int argc, char *argv[])
{
  if (ByteOrder::platformByteOrder() == ByteOrder::LittleEndian)
    printf("LittleEndian\n");
  else
    printf("BigEndian\n");

  i16 lU16 = 0x1234;
  i32 lU32 = 0x12345678;
  i64 lU64 = 0x0123456789ABCDEF;

  ByteOrder::NetSwap(lU16);
  printf("swap(lU16) = 0x%04X\n", lU16);
  ByteOrder::NetSwap(lU32);
  printf("swap(lU32) = 0x%08X\n", lU32);
  ByteOrder::NetSwap(lU64);
  printf("swap(lU64) = 0x%08X%08X\n", lU64 >> 32, lU64);

  return 0;
}

