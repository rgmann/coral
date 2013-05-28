#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sstream>
#include <iostream>
#include <string>
#include "SegmenterUnit.h"
#include "AuthorityUnit.h"
#include "AssemblerUnit.h"


int main(int argc, char *argv[])
{
   SegmenterUnit* l_pSegmenterUnit = NULL;
   AuthorityUnit* l_pAuthorityUnit = NULL;
   AssemblerUnit* l_pAssemblerUnit = NULL;
   
   std::string rootPathClient = "flowtestdata/client_dir";
   std::string rootPathServer = "flowtestdata/server_dir";
   std::string filename = "file.dat";
   
   Queue<RsyncPacket*> outQueue;
   
   printf("class test\n");
   
   // Initialize the output queue as an "infinite" queue.
   outQueue.initialize();

   l_pSegmenterUnit = new SegmenterUnit(outQueue);
   l_pAuthorityUnit = new AuthorityUnit(outQueue);
   l_pAssemblerUnit = new AssemblerUnit(outQueue);
   
   // Set the segment size in bytes.
   l_pSegmenterUnit->setSegmentSize(6);
   
   // Provide the assembler with a pointer to the segmenter.
   l_pAssemblerUnit->setSegmenter(l_pSegmenterUnit->getSegmenter());
   
   // Register each unit for a packet type.
   l_pSegmenterUnit->initialize(RsyncPacket::TypeNotSet);
   l_pAuthorityUnit->initialize(RsyncPacket::SegmentReportType);
   l_pAssemblerUnit->initialize(RsyncPacket::AssemblyInstType);
   
   // Set the root path for the Segmenter and the Autority.
   l_pSegmenterUnit->setRoot(rootPathClient);
   l_pAuthorityUnit->setRoot(rootPathServer);
   
   // Add a file to the Segmenter's queue.
   l_pSegmenterUnit->addFilename(filename);
   
   do {
      
      // Route any packets in the output queue back to the units.
      if (!outQueue.isEmpty())
      {
         RsyncPacket* l_pPacket = NULL;
         
         // Pull a packet from the queue.
         if (outQueue.pop(l_pPacket, 100))
         {
            WorkUnit::Route(l_pPacket);
         }
      }
      
      // Give each unit a turn at working.
      l_pSegmenterUnit->work();
      l_pAuthorityUnit->work();
      l_pAssemblerUnit->work();
      
   } while (!outQueue.isEmpty());
   
   printf("\nDone!\n");
   
   delete l_pAssemblerUnit;
   delete l_pAuthorityUnit;
   delete l_pSegmenterUnit;
   
   return 0;
}
