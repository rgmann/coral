#include "RsyncSegmenter.h"
#include "RsyncFileAuthority.h"
#include "RsyncAssembler.h"
#include "FileSystemHelper.h"
#include <stdio.h>
#include <unistd.h>
#include <sstream>
#include <iostream>

int main(int argc, char *argv[])
{
   std::string    filename;
   
   RsyncSegmenter       segmenter;
   RsyncSegmentReport*  pReport = NULL;
   //RsyncReportHeader    reportHeader;
   RsyncSegmentReportHdr*  pReportHdr = NULL;
   //RsyncPackedSeg       packedSeg;
   RsyncSegmentPacket*     pSegment = NULL;
   
   
   RsyncSegmentTable    table;
   int                  l_nSegCount = 0;
   
   RsyncFileAuthority   authority;
   
   
   RsyncAssembler       assembler;
   RsyncAssemblyInstr*  pInstr = NULL;
      
   printf("Flow Test:\n");
   
   //filename = "2Scan_120800003.jpg";
   filename = "file.dat";
   //filename = "firefox2005-icon.png";
   
   // Begin by segmenting the clientfile
   segmenter.setRoot("flowtestdata/client_dir");
   segmenter.setSegmentSize(6);
   
   if (!segmenter.process(filename))
   {
      std::cout << "Failed to segment client file." << std::endl;
      return 1;
   }
   //return 1;
   
   // Once processing is complete, we can retrieve the segmenation report.
   pReport = segmenter.getReport(filename);
   if (pReport == NULL)
   {
      std::cout << "Failed to get Segmentation Report" << std::endl;
      return 1;
   }
   
//   if (!pReport->header(&reportHeader))
   if (!pReport->header(&pReportHdr))
   {
      std::cout << "Failed to get Segmentation Report header" << std::endl;
      return 1;
   }
//   RsyncSegmentReport::PrintReportHeader(&reportHeader);
   RsyncSegmentReport::PrintReportHeader(pReportHdr);
   
   
   // Even though the assembler will not be doing anything for a while,
   // we can give it a reference to the segmenter so that it can retrieve the
   // report when necessary (in fact this should done once at startup).
   assembler.setSegmenter(&segmenter);
   
   // "Send" the report header and packed segments to the hash table on the
   // server.
//   table.setHeader(&reportHeader);
   table.addSegment(pReportHdr);
   //while (pReport->nextPackedSeg(&packedSeg))
   while (pReport->nextSegment(&pSegment))
   {
      //if (!table.addSegment(&packedSeg))
      RsyncSegmentReportPacket* l_pSegRprtPkt = NULL;
      l_pSegRprtPkt = reinterpret_cast<RsyncSegmentReportPacket*>(pSegment);
      if (l_pSegRprtPkt->type() == RsyncSegmentReportPacket::SegmentType) {
         std::cout << "Next segment type = SegmentType" << std::endl;
      }
      else if (l_pSegRprtPkt->type() == RsyncSegmentReportPacket::HeaderType) {
         std::cout << "Next segment type = HeaderType" << std::endl;
      }
      else {
         std::cout << "Next segment type = " << l_pSegRprtPkt->type()
                  << std::endl;
      }
      if (!table.addSegment(pSegment))
         std::cout << "Failed to add seg #" << l_nSegCount << std::endl;
      l_nSegCount++;
      
      pSegment = NULL;
   }
   std::cout << "Added " << l_nSegCount << " segments to the table."
            << std::endl;
   
   // Check that we have the complete segment list.
   if (!table.isComplete())
   {
      std::cout << "RsyncHashTable received fewer than expected segments."
                << std::endl;
      return 1;
   }
   
   return 1;
   
   // Pass the hash to the Authority
   authority.setRoot("flowtestdata/server_dir");
   if (!authority.process(&table))
   {
      std::cout << "Authority failed to process segment hash."
                << std::endl;
      return 1;
   }
   
   /*
   if (!authority.getMarker(&marker))
   {
      std::cout << "Failed to get re-assembly marker."
                << std::endl;
      return 1;
   }
    */
   
   // The the file marker and re-assembly instructions back to the client.
   RsyncAssembler::AddStatus status = RsyncAssembler::Failure;
   assembler.start(RsyncAssembler::NoStaging);
   while (authority.getNextInstruction(&pInstr))
   {
      unsigned char *l_pPackedInstr = NULL;
      unsigned int l_nPackedInstrLen = 0;
      
      if (pInstr == NULL) {
         std::cout << "Null instruction pointer" << std::endl;
         return 1;
      }
      
      pInstr->pack((void**)&l_pPackedInstr, l_nPackedInstrLen);
      
      if (l_pPackedInstr == NULL)
      {
         std::cout << "Null packed instruction pointer" << std::endl;
         return 1;
      }
      
      status = assembler.addInstruction(l_pPackedInstr, l_nPackedInstrLen);
      if (status == RsyncAssembler::Failure)
      {
         std::cout << "Failed to add instruction" << std::endl;
         return 1;
      }
      
      delete[] l_pPackedInstr;
   }
   
   
   // Since we specified NoStaging, the instructions were automatically
   // executed as they were added.  Therefore, calling "assemble" simply
   // replaces the real file with the stage file.
   if (!assembler.assemble())
   {
      std::cout << "Failed to assemble file." << std::endl;
      return 1;
   }
   
   std::cout << "Done!" << std::endl;
   
   return 0;
}

