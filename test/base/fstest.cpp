//#include "FileNode.h"
#include "FileNodeMonitor.h"
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

int main(int argc, char *argv[])
{
   std::string dir(".");
   struct sigaction sigIntHandler;
   
   FileNode       locNode;
   std::string    path;
   FileNameList   ignoreNames;
   FileNameList   hideNames;
   
   FileNodeMonitor monitor;//(&locNode);
   FileNodeList    addedFiles;
   
   printf("File System test:\r\n");
   
   if (argc > 1)
   {
      path = argv[1];
   }
   else
   {
      printf("Wrong number of args\n");
      return 1;
   }
   
   sigIntHandler.sa_handler = sighandler;
   sigemptyset(&sigIntHandler.sa_mask);
   sigIntHandler.sa_flags = 0;
   
   sigaction(SIGINT, &sigIntHandler, NULL);

   ignoreNames.push_back(".");
   ignoreNames.push_back("..");
   FileNode::Traverse(path, locNode, ignoreNames, true);
   
   FileNode::Print(locNode);
   
   printf("\n");
   
   return 0;
}
