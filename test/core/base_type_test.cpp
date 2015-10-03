#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include "BaseTypes.h"

int main(int argc, char *argv[])
{
   std::cout << "sizeof(i8)," << sizeof(i8) << std::endl;
   std::cout << "sizeof(u8)," << sizeof(ui8) << std::endl;
   std::cout << "sizeof(i16)," << sizeof(i16) << std::endl;
   std::cout << "sizeof(ui16)," << sizeof(ui16) << std::endl;
   std::cout << "sizeof(i32)," << sizeof(i32) << std::endl;
   std::cout << "sizeof(ui32)," << sizeof(ui32) << std::endl;
   std::cout << "sizeof(i64)," << sizeof(i64) << std::endl;
   std::cout << "sizeof(ui64)," << sizeof(ui64) << std::endl;
   return 0;
}
