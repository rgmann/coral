//#include "FileNode.h"
#include "DsvFileReader.h"
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string>

bool  g_bStopCommanded = false;

void sighandler(int s)
{
   exit(1);
}

void PrintVector(const std::vector<std::string> &vec, char delim);

int main(int argc, char *argv[])
{
   std::string dir(".");
   struct sigaction sigIntHandler;
   
   std::vector<std::string> row;
   std::string    path;
   DsvFileReader  fileReader;
   
   printf("Delimiter Separated File test:\r\n");
   
   if (argc > 1)
   {
      path = argv[1];
   }
   else
   {
      printf("Wrong number of args\n");
      return 1;
   }
   
   if (!fileReader.open(path, ',', true))
   {
      printf("Failed to open file.\n");
      return 1;
   }
   
   while (fileReader.readRow(row))
   {
      PrintVector(row, ',');
   }
   fileReader.close();
   
   return 0;
}

void PrintVector(const std::vector<std::string> &vec, char delim)
{
   std::vector<std::string>::const_iterator it = vec.begin();
   
   if (vec.empty()) {
      return;
   }
   
   for (; it < vec.end(); ++it) {
      printf("%s%c", it->c_str(), delim);
   }
   printf("\n");
}
