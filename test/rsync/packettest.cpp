#include <stdio.h>
#include <unistd.h>
#include <sstream>
#include <iostream>
#include "RsyncPacket.h"
#include "RsyncSegmentReportPacket.h"
#include "RsyncSegmentReportHdr.h"

//using namespace Rsync;

bool testRsyncPacket();
bool testRsyncSegmentReportPacket();
bool testSegmentReportHeader();
bool testSegmentPacket();

int main(int argc, char *argv[])
{
   printf("Rsync Packet test\n");
   
   if (!testRsyncPacket())
   {
      printf("testRsyncPacket [FAIL]\n");
   }
   else
   {
      printf("testRsyncPacket [PASS]\n");
   }
   
   if (!testRsyncSegmentReportPacket())
   {
      printf("testRsyncSegmentReportPacket [FAIL]\n");
   }
   else
   {
      printf("testRsyncSegmentReportPacket [PASS]\n");
   }
   
   if (!testSegmentReportHeader())
   {
      printf("testSegmentReportHeader [FAIL]\n");
   }
   else
   {
      printf("testSegmentReportHeader [PASS]\n");
   }
   
   if (!testSegmentPacket())
   {
      printf("testSegmentPacket [FAIL]\n");
   }
   else
   {
      printf("testSegmentPacket [PASS]\n");
   }
   
   return 0;
}

bool testRsyncPacket()
{
   RsyncPacket::Data* l_pPktHdr = NULL;
   char* l_pPackedPkt = NULL;
   ui32  l_nPktSize = 0;
   
   RsyncPacket*   l_pTxPacket = NULL;
   RsyncPacket    l_RxPacket;
   
   l_pTxPacket = new RsyncPacket(RsyncPacket::SegmentReportType, 0);
   if (l_pTxPacket == NULL)
   {
      printf("testRsyncPacket: failure at line #%d\n", __LINE__);
      return false;
   }
   
   if (l_pTxPacket->type() != RsyncPacket::SegmentReportType)
   {
      printf("testRsyncPacket: failure at line #%d\n", __LINE__);
      return false;
   }
   
   if (!l_pTxPacket->pack((void**)&l_pPackedPkt, l_nPktSize))
   {
      printf("testRsyncPacket: failure at line #%d\n", __LINE__);
      return false;
   }
   
   if (l_pPackedPkt == NULL)
   {
      printf("testRsyncPacket: failure at line #%d\n", __LINE__);
      return false;
   }
   
   if (l_nPktSize != sizeof(RsyncPacket::Data))
   {
      printf("testRsyncPacket: failure at line #%d\n", __LINE__);
      return false;
   }
   
   l_pPktHdr = (RsyncPacket::Data*)l_pPackedPkt;
   if (l_pPktHdr->marker != RsyncPacket::Data::marker)
   {
      printf("testRsyncPacket: failure at line #%d\n", __LINE__);
      return false;
   }
   
   if (l_pPktHdr->type != RsyncPacket::SegmentReportType)
   {
      printf("testRsyncPacket: failure at line #%d\n", __LINE__);
      return false;
   }
   
   if (l_pPktHdr->length != 0)
   {
      printf("testRsyncPacket: failure at line #%d\n", __LINE__);
      return false;
   }
   
   if (!l_RxPacket.unpack(l_pPackedPkt, l_nPktSize))
   {
      printf("testRsyncPacket: failure at line #%d\n", __LINE__);
      return false;
   }
   
   if (l_RxPacket.type() != RsyncPacket::SegmentReportType)
   {
      printf("testRsyncPacket: failure at line #%d\n", __LINE__);
      return false;
   }
   
   delete[] l_pPackedPkt; l_pPackedPkt = NULL;
   delete l_pTxPacket; l_pTxPacket = NULL;
   
   
   
   l_pTxPacket = new RsyncPacket(RsyncPacket::AssemblyInstType, 0);
   if (l_pTxPacket == NULL)
   {
      printf("testRsyncPacket: failure at line #%d\n", __LINE__);
      return false;
   }
   
   if (l_pTxPacket->type() != RsyncPacket::AssemblyInstType)
   {
      printf("testRsyncPacket: failure at line #%d\n", __LINE__);
      return false;
   }
   
   if (!l_pTxPacket->pack((void**)&l_pPackedPkt, l_nPktSize))
   {
      printf("testRsyncPacket: failure at line #%d\n", __LINE__);
      return false;
   }
   
   if (l_pPackedPkt == NULL)
   {
      printf("testRsyncPacket: failure at line #%d\n", __LINE__);
      return false;
   }
   
   if (l_nPktSize != sizeof(RsyncPacket::Data))
   {
      printf("testRsyncPacket: failure at line #%d\n", __LINE__);
      return false;
   }
   
   l_pPktHdr = (RsyncPacket::Data*)l_pPackedPkt;
   if (l_pPktHdr->marker != RsyncPacket::Data::marker)
   {
      printf("testRsyncPacket: failure at line #%d\n", __LINE__);
      return false;
   }
   
   if (l_pPktHdr->type != RsyncPacket::AssemblyInstType)
   {
      printf("testRsyncPacket: failure at line #%d\n", __LINE__);
      return false;
   }
   
   if (l_pPktHdr->length != 0)
   {
      printf("testRsyncPacket: failure at line #%d\n", __LINE__);
      return false;
   }
   
   if (!l_RxPacket.unpack(l_pPackedPkt, l_nPktSize))
   {
      printf("testRsyncPacket: failure at line #%d\n", __LINE__);
      return false;
   }
   
   if (l_RxPacket.type() != RsyncPacket::AssemblyInstType)
   {
      printf("testRsyncPacket: failure at line #%d\n", __LINE__);
      return false;
   }
   
   delete[] l_pPackedPkt; l_pPackedPkt = NULL;
   delete l_pTxPacket; l_pTxPacket = NULL;
   
   return true;
}

//------------------------------------------------------------------------------
bool testRsyncSegmentReportPacket()
{
   RsyncPacket::Data* l_pPktHdr = NULL;
   RsyncSegmentReportPacket::Data* l_pSubPktHdr = NULL;
   char* l_pPackedPkt = NULL;
   ui32  l_nPktSize = 0;
   
   RsyncPacket*   l_pSuperPkt = NULL;
   RsyncSegmentReportPacket* l_pTxPacket = NULL;
   RsyncPacket    l_RxPacket;
   RsyncSegmentReportPacket l_RxSubPkt;
   
   l_pTxPacket = new RsyncSegmentReportPacket(RsyncSegmentReportPacket::
                                              HeaderType, 0);
   if (l_pTxPacket == NULL)
   {
      printf("testRsyncSegmentReportPacket: failure at line #%d\n", __LINE__);
      return false;
   }
   
   if (l_pTxPacket->type() != RsyncSegmentReportPacket::HeaderType)
   {
      printf("testRsyncSegmentReportPacket: failure at line #%d\n", __LINE__);
      return false;
   }
   
   l_pSuperPkt = dynamic_cast<RsyncPacket*>(l_pTxPacket);
   if (l_pSuperPkt->type() != RsyncPacket::SegmentReportType)
   {
      printf("testRsyncSegmentReportPacket: failure at line #%d\n", __LINE__);
      return false;
   }
   
   if (!l_pTxPacket->pack((void**)&l_pPackedPkt, l_nPktSize))
   {
      printf("testRsyncSegmentReportPacket: failure at line #%d\n", __LINE__);
      return false;
   }
   
   if (l_pPackedPkt == NULL)
   {
      printf("testRsyncSegmentReportPacket: failure at line #%d\n", __LINE__);
      return false;
   }
   
   ui32 l_nExpectedSize =  sizeof(RsyncPacket::Data) +
                           sizeof(RsyncSegmentReportPacket::Data);
   
   if (l_nPktSize != l_nExpectedSize)
   {
      printf("testRsyncSegmentReportPacket: failure at line #%d\n", __LINE__);
      return false;
   }
   
   l_pPktHdr = (RsyncPacket::Data*)l_pPackedPkt;
   if (l_pPktHdr->marker != RsyncPacket::Data::marker)
   {
      printf("testRsyncSegmentReportPacket: failure at line #%d\n", __LINE__);
      return false;
   }
   
   if (l_pPktHdr->type != RsyncPacket::SegmentReportType)
   {
      printf("testRsyncSegmentReportPacket: failure at line #%d\n", __LINE__);
      return false;
   }
   
   if (l_pPktHdr->length != sizeof(RsyncSegmentReportPacket::Data))
   {
      printf("testRsyncSegmentReportPacket: failure at line #%d\n", __LINE__);
      return false;
   }
   
   l_pSubPktHdr = (RsyncSegmentReportPacket::Data*)(l_pPackedPkt +
                                                    sizeof(RsyncPacket::Data));
   if (l_pSubPktHdr->type != RsyncSegmentReportPacket::HeaderType)
   {
      printf("testRsyncSegmentReportPacket: failure at line #%d\n", __LINE__);
      return false;
   }
   
   if (l_pSubPktHdr->length != 0)
   {
      printf("testRsyncSegmentReportPacket: failure at line #%d\n", __LINE__);
      return false;
   }
   
   if (!l_RxPacket.unpack(l_pPackedPkt, l_nPktSize))
   {
      printf("testRsyncSegmentReportPacket: failure at line #%d\n", __LINE__);
      return false;
   }
   
   if (l_RxPacket.type() != RsyncPacket::SegmentReportType)
   {
      printf("testRsyncSegmentReportPacket: failure at line #%d\n", __LINE__);
      return false;
   }
   
   if (!l_RxSubPkt.unpack(l_pPackedPkt, l_nPktSize))
   {
      printf("testRsyncSegmentReportPacket: failure at line #%d\n", __LINE__);
      return false;
   }
   
   if (l_RxSubPkt.type() != RsyncSegmentReportPacket::HeaderType)
   {
      printf("testRsyncSegmentReportPacket: failure at line #%d\n", __LINE__);
      return false;
   }
   
   delete[] l_pPackedPkt; l_pPackedPkt = NULL;
   delete l_pTxPacket; l_pTxPacket = NULL;
   
   return true;
}

//------------------------------------------------------------------------------
bool testSegmentReportHeader()
{
   RsyncPacket::Data* l_pPktHdr = NULL;
   RsyncSegmentReportPacket::Data* l_pSubPktHdr = NULL;
   RsyncSegmentReportHdr::Data* l_pPktData = NULL;
   char* l_pPackedPkt = NULL;
   ui32  l_nPktSize = 0;
   
   std::string l_sFilename = "test_file.txt";
   ui32  l_nSegmentCount = 100;
   ui32  l_nSegmentSize = 128;
   
   RsyncSegmentReportHdr*     l_pTxPacket = NULL;
   RsyncSegmentReportPacket*  l_pTxPrntPkt = NULL;
   RsyncPacket*               l_pTxBasePkt = NULL;
   
   RsyncSegmentReportHdr      l_RxPkt;
   RsyncSegmentReportPacket   l_RxPrntPkt;
   RsyncPacket                l_RxBasePkt;
   
   l_pTxPacket = new RsyncSegmentReportHdr();
   if (l_pTxPacket == NULL)
   {
      printf("testSegmentReportHeader: failure at line #%d\n", __LINE__);
      return false;
   }
   
   if (l_pTxPacket->type() != RsyncSegmentReportPacket::HeaderType)
   {
      printf("testSegmentReportHeader: failure at line #%d\n", __LINE__);
      return false;
   }
   
   if (!l_pTxPacket->setFilePath(l_sFilename))
   {
      printf("testSegmentReportHeader: failure at line #%d\n", __LINE__);
      return false;
   }
   
   if (!l_pTxPacket->setSegmentCount(l_nSegmentCount))
   {
      printf("testSegmentReportHeader: failure at line #%d\n", __LINE__);
      return false;
   }
   
   if (!l_pTxPacket->setSegmentSize(l_nSegmentSize))
   {
      printf("testSegmentReportHeader: failure at line #%d\n", __LINE__);
      return false;
   }
   
   l_pTxPrntPkt = dynamic_cast<RsyncSegmentReportPacket*>(l_pTxPacket);
   if (l_pTxPrntPkt->type() != RsyncSegmentReportPacket::HeaderType)
   {
      printf("testSegmentReportHeader: failure at line #%d\n", __LINE__);
      return false;
   }
   
   l_pTxBasePkt = dynamic_cast<RsyncPacket*>(l_pTxPacket);
   if (l_pTxBasePkt->type() != RsyncPacket::SegmentReportType)
   {
      printf("testSegmentReportHeader: failure at line #%d\n", __LINE__);
      return false;
   }
   
   if (!l_pTxPacket->pack((void**)&l_pPackedPkt, l_nPktSize))
   {
      printf("testSegmentReportHeader: failure at line #%d\n", __LINE__);
      return false;
   }
   
   if (l_pPackedPkt == NULL)
   {
      printf("testSegmentReportHeader: failure at line #%d\n", __LINE__);
      return false;
   }
   
   ui32 l_nExpectedSize = sizeof(RsyncPacket::Data) +
                     sizeof(RsyncSegmentReportPacket::Data) +
                     sizeof(RsyncSegmentReportHdr::Data);
   if (l_nPktSize != l_nExpectedSize)
   {
      printf("testSegmentReportHeader: failure at line #%d\n", __LINE__);
      return false;
   }
   
   l_pPktHdr = (RsyncPacket::Data*)l_pPackedPkt;
   if (l_pPktHdr->marker != RsyncPacket::Data::marker)
   {
      printf("testRsyncSegmentReportPacket: failure at line #%d\n", __LINE__);
      return false;
   }
   
   if (l_pPktHdr->type != RsyncPacket::SegmentReportType)
   {
      printf("testRsyncSegmentReportPacket: failure at line #%d\n", __LINE__);
      return false;
   }
   
   l_nExpectedSize = sizeof(RsyncSegmentReportPacket::Data) +
                     sizeof(RsyncSegmentReportHdr::Data);
   if (l_pPktHdr->length != sizeof(RsyncSegmentReportPacket::Data))
   {
      printf("testRsyncSegmentReportPacket: failure at line #%d\n", __LINE__);
      return false;
   }
   
   l_pSubPktHdr = (RsyncSegmentReportPacket::Data*)(l_pPackedPkt +
                                                    sizeof(RsyncPacket::Data));
   if (l_pPktHdr->type != RsyncSegmentReportPacket::HeaderType)
   {
      printf("testRsyncSegmentReportPacket: failure at line #%d\n", __LINE__);
      return false;
   }
   
   l_nExpectedSize = sizeof(RsyncSegmentReportHdr::Data);
   if (l_pPktHdr->length != l_nExpectedSize)
   {
      printf("testRsyncSegmentReportPacket: failure at line #%d\n", __LINE__);
      return false;
   }
   
   l_pPktData = (RsyncSegmentReportPacket::Data*)(l_pPackedPkt +
                                       sizeof(RsyncPacket::Data) +
                                       sizeof(RsyncSegmentReportHdr::Data));
   if (strncmp(l_pPktData->filePath, l_sFilename.c_str(),
               RsyncMaxPathLength) != 0)
   {
      printf("testRsyncSegmentReportPacket: failure at line #%d\n", __LINE__);
      return false;
   }
   
   if (l_pPktData->segmentCount != l_nSegmentCount)
   {
      printf("testRsyncSegmentReportPacket: failure at line #%d\n", __LINE__);
      return false;
   }
   
   if (l_pPktData->segmentSizeBytes != l_nSegmentSize)
   {
      printf("testRsyncSegmentReportPacket: failure at line #%d\n", __LINE__);
      return false;
   }
   
   if (!l_RxPacket.unpack(l_pPackedPkt, l_nPktSize))
   {
      printf("testRsyncSegmentReportPacket: failure at line #%d\n", __LINE__);
      return false;
   }
   
   if (l_RxPacket.type() != RsyncPacket::SegmentReportType)
   {
      printf("testRsyncSegmentReportPacket: failure at line #%d\n", __LINE__);
      return false;
   }
   
   if (!l_RxSubPkt.unpack(l_pPackedPkt, l_nPktSize))
   {
      printf("testRsyncSegmentReportPacket: failure at line #%d\n", __LINE__);
      return false;
   }
   
   if (l_RxSubPkt.type() != RsyncSegmentReportPacket::HeaderType)
   {
      printf("testRsyncSegmentReportPacket: failure at line #%d\n", __LINE__);
      return false;
   }
   
   delete[] l_pPackedPkt; l_pPackedPkt = NULL;
   delete l_pTxPacket; l_pTxPacket = NULL;
   
   return true;
}

//------------------------------------------------------------------------------
bool testSegmentPacket()
{
   return true;
}
