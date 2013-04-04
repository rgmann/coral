#include "VectorClock.h"
#include <iostream>
#include <string>
#include <stdio.h>
#include <unistd.h>


int main(int argc, char *argv[])
{
   VectorClock  locVC1;
   VectorClock  locVC2;
   
   std::string  locJson1;
      
   printf("VectorClock test\r\n");
   
   locVC1.initialize(0, 5);
   
   std::cout << "JSON: " << locVC1.toJson() << std::endl;
   
   locVC1.increment();
   std::cout << "JSON: " << locVC1.toJson() << std::endl;
   locJson1 = locVC1.toJson();
   
   locVC2.fromJson(locJson1);
   std::cout << "JSON: " << locVC2.toJson() << std::endl;
   
   return 0;
}

