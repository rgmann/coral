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
   g_bStopCommanded = true;  
}

const char* deltaTypeToString(FileNodeMonitor::DeltaType type);

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
   
   printf("Monitor test:\r\n");
   
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

   monitor.setBase(path);
   
   while (!g_bStopCommanded)
   {
      FileNodeMonitor::FileSysDelta delta;
      std::vector<FileNode>::iterator it;
      sleep(1);
      
      monitor.refresh();
      //addedFiles = monitor.getAddedNodes();
//      
//      it = addedFiles.begin();
//      for (; it < addedFiles.end(); ++it) {
//         FileNode::Print(*it);
//      }
      while (monitor.getDelta(delta, 100)) {
         std::cout << deltaTypeToString(delta.type) << " ";
         FileNode::Print(delta.node);
      }
   }
   
   printf("\n");
   
   return 0;
}

const char* deltaTypeToString(FileNodeMonitor::DeltaType type)
{
   switch (type) {
      case FileNodeMonitor::Added: return "ADDED";
      case FileNodeMonitor::Removed: return "REMOVED";
      case FileNodeMonitor::Modified: return "MODIFIED";
      case FileNodeMonitor::Updated: return "UPDATED";
      default: return "OTHER";
   }
}
