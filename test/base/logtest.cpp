#include <stdio.h>
#include <unistd.h>
#include <iostream>
#include "Log.h"
#include "Timestamp.h"


#define LOG_COUNT 1000

int main(int argc, char *argv[])
{
   char l_cChar = 0;
   
   
   Timestamp start;
   Timestamp stop;
   
   f32 lElapsedTimeUs = 0;
      
   printf("Log test\n");
   
   Log::Setup("logtest", "logtestdir", true);
   
   Log::PrintToConsole(true);
   
   WARN("Connection attempted " << 10 << " times.");
   
   TRACE("Test trace!");
   
   LOG(Log::Error << "Failed to connect to server!");
   
   Log::PrintToConsole(false);

   start.sample();
   for (int lnCount = 0; lnCount < LOG_COUNT; lnCount++)
   {
      INFO("Test string " << 10);
   }
   stop.sample();
   lElapsedTimeUs = (f32)stop.diffInUs(start);
   
   std::cout << "To file time: " 
             << (lElapsedTimeUs/(f32)LOG_COUNT) << std::endl;
   
   start.sample();
   for (int lnCount = 0; lnCount < LOG_COUNT; lnCount++)
   {
      TRACE("Test string " << 10);
   }
   stop.sample();
   lElapsedTimeUs = (f32)stop.diffInUs(start);
   
   std::cout << "Trace time: " 
             << (lElapsedTimeUs/(f32)LOG_COUNT) << std::endl;
   
   Log::Teardown();

   return 0;
}

