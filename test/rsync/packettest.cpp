#include <stdio.h>
#include <unistd.h>
#include <sstream>
#include <iostream>
#include "RsyncPacket.h"
//#include "RsyncSegmentReportPacket.h"
#include "RsyncSegmentReportHdr.h"

//using namespace Rsync;

bool testAssemblyBeginMarker();
bool testAssemblyEndMarker();
bool testAssemblyChunkPacket();
bool testAssemblySegment();
bool testRsyncSegmentReportHdr();
bool testRsyncSegmentPacket();

int main(int argc, char *argv[])
{
   printf("Rsync Packet test\n");
   
   if (!testAssemblyBeginMarker())
   {
      printf("testAssemblyBeginMarker [FAIL]\n");
   }
   else
   {
      printf("testAssemblyBeginMarker [PASS]\n");
   }
   
   if (!testAssemblyEndMarker())
   {
      printf("testAssemblyEndMarker [FAIL]\n");
   }
   else
   {
      printf("testAssemblyEndMarker [PASS]\n");
   }
   
   if (!testAssemblyChunkPacket())
   {
      printf("testAssemblyChunkPacket [FAIL]\n");
   }
   else
   {
      printf("testAssemblyChunkPacket [PASS]\n");
   }
   
   if (!testAssemblySegment())
   {
      printf("testAssemblySegment [FAIL]\n");
   }
   else
   {
      printf("testAssemblySegment [PASS]\n");
   }
   
   if (testRsyncSegmentReportHdr())
   {
      printf("testRsyncSegmentPacket [PASS]\n");
   }
   else
   {
      printf("testRsyncSegmentPacket [PASS]\n");
   }
   
   return 0;
}

//------------------------------------------------------------------------------
bool testAssemblyBeginMarker()
{
   AssemblyBeginMarker* pBeginMarker = NULL;
   RsyncAssemblyInstr* pInstr = NULL;
   
   char* l_pPackedPkt = NULL;
   ui32  l_nPktSize = 0;
   
   std::string testString = "test string";
   pBeginMarker = new AssemblyBeginMarker(testString);
   pInstr = new RsyncAssemblyInstr(pBeginMarker);
      
   if (pInstr->type() != RsyncAssemblyInstr::BeginMarkerType)
   {
      printf("testAssemblyBeginMarker: failure at line #%d\n", __LINE__);
      return false;
   }
   
   if (!pInstr->pack((void**)&l_pPackedPkt, l_nPktSize))
   {
      printf("testAssemblyBeginMarker: failure at line #%d\n", __LINE__);
      return false;
   }
   
   if (l_pPackedPkt == NULL)
   {
      printf("testAssemblyBeginMarker: failure at line #%d\n", __LINE__);
      return false;
   }
   
   if (l_nPktSize != (sizeof(RsyncAssemblyInstr::Data) + testString.length()))
   {
      printf("testAssemblyBeginMarker: failure at line #%d\n", __LINE__);
      return false;
   }
   
   delete pInstr; pInstr = NULL;
   delete pBeginMarker; pBeginMarker = NULL;
   
   pInstr = new RsyncAssemblyInstr();
   
   if (!pInstr->unpack(l_pPackedPkt, l_nPktSize))
   {
      printf("testAssemblyBeginMarker: failure at line #%d\n", __LINE__);
      return false;
   }
   
   if (!pInstr->to(&pBeginMarker))
   {
      printf("testAssemblyBeginMarker: failure at line #%d\n", __LINE__);
      return false;
   }
   
   if (pBeginMarker == NULL)
   {
      printf("testAssemblyBeginMarker: failure at line #%d\n", __LINE__);
      return false;
   }
   
   std::string rxFilePath = "";
   if (!pBeginMarker->getPath(rxFilePath))
   {
      printf("testAssemblyBeginMarker: failure at line #%d\n", __LINE__);
      return false;
   }
   
   if (testString != rxFilePath)
   {
      printf("testAssemblyBeginMarker: failure at line #%d\n", __LINE__);
      return false;
   }
   
   delete[] l_pPackedPkt; l_pPackedPkt = NULL;
   delete pBeginMarker; pBeginMarker = NULL;
   delete pInstr; pInstr = NULL;
   
   return true;
}

//------------------------------------------------------------------------------
bool testAssemblyEndMarker()
{
   RsyncAssemblyInstr* pInstr = NULL;
   AssemblyEndMarker* pEndMarker = NULL;
   
   char* l_pPackedPkt = NULL;
   ui32  l_nPktSize = 0;
   
   pEndMarker = new AssemblyEndMarker();
   pInstr = new RsyncAssemblyInstr(pEndMarker);
   
   if (pInstr->type() != RsyncAssemblyInstr::EndMarkerType)
   {
      printf("testAssemblyEndMarker: failure at line #%d\n", __LINE__);
      return false;
   }
   
   if (!pInstr->pack((void**)&l_pPackedPkt, l_nPktSize))
   {
      printf("testAssemblyEndMarker: failure at line #%d\n", __LINE__);
      return false;
   }
   
   if (l_pPackedPkt == NULL)
   {
      printf("testAssemblyEndMarker: failure at line #%d\n", __LINE__);
      return false;
   }
   
   if (l_nPktSize != (sizeof(RsyncAssemblyInstr::Data)))
   {
      printf("testAssemblyEndMarker: failure at line #%d\n", __LINE__);
      return false;
   }
   
   delete pInstr; pInstr = NULL;
   delete pEndMarker; pEndMarker = NULL;
   
   pInstr = new RsyncAssemblyInstr();
   
   if (!pInstr->unpack(l_pPackedPkt, l_nPktSize))
   {
      printf("testAssemblyEndMarker: failure at line #%d\n", __LINE__);
      return false;
   }
   
   if (pInstr->type() != RsyncAssemblyInstr::EndMarkerType)
   {
      printf("testAssemblyEndMarker: failure at line #%d\n", __LINE__);
      return false;
   }
   
   if (!pInstr->to(&pEndMarker))
   {
      printf("testAssemblyEndMarker: failure at line #%d\n", __LINE__);
      return false;
   }
   
   if (pEndMarker == NULL)
   {
      printf("testAssemblyEndMarker: failure at line #%d\n", __LINE__);
      return false;
   }
   
   delete[] l_pPackedPkt; l_pPackedPkt = NULL;
   delete pEndMarker; pEndMarker = NULL;
   delete pInstr; pInstr = NULL;
   
   return true;
}

//------------------------------------------------------------------------------
bool testAssemblyChunkPacket()
{
   static const ui8 Pattern[] = { 0x0F, 0x1E, 0x2D, 0x3C, 0x4B, 0x5A, 0x69,
                                  0x78, 0x87, 0x96, 0xA5, 0xB4, 0xC3, 0xD2,
                                  0xE1, 0xF0 };
   
   RsyncAssemblyInstr* pInstr = NULL;
   AssemblyChunkPacket* pChunkPkt = NULL;
   
   char* l_pPackedPkt = NULL;
   ui32  l_nPktSize = 0;
   
   pChunkPkt = new AssemblyChunkPacket(sizeof(Pattern));
   memcpy(pChunkPkt->data(), Pattern, sizeof(Pattern));
   
   pInstr = new RsyncAssemblyInstr(pChunkPkt);
   
   if (pInstr->type() != RsyncAssemblyInstr::ChunkType)
   {
      printf("testAssemblyChunkPacket: failure at line #%d\n", __LINE__);
      return false;
   }
   
   if (!pInstr->pack((void**)&l_pPackedPkt, l_nPktSize))
   {
      printf("testAssemblyChunkPacket: failure at line #%d\n", __LINE__);
      return false;
   }
   
   if (l_pPackedPkt == NULL)
   {
      printf("testAssemblyChunkPacket: failure at line #%d\n", __LINE__);
      return false;
   }
   
   if (l_nPktSize != (sizeof(RsyncAssemblyInstr::Data) + sizeof(Pattern)))
   {
      printf("testAssemblyChunkPacket: failure at line #%d\n", __LINE__);
      return false;
   }
   
   delete pInstr; pInstr = NULL;
   delete pChunkPkt; pChunkPkt = NULL;
   
   pInstr = new RsyncAssemblyInstr();
   
   if (!pInstr->unpack(l_pPackedPkt, l_nPktSize))
   {
      printf("testAssemblyChunkPacket: failure at line #%d\n", __LINE__);
      return false;
   }
   
   if (pInstr->type() != RsyncAssemblyInstr::ChunkType)
   {
      printf("testAssemblyChunkPacket: failure at line #%d\n", __LINE__);
      return false;
   }
   
   if (!pInstr->to(&pChunkPkt))
   {
      printf("testAssemblyChunkPacket: failure at line #%d\n", __LINE__);
      return false;
   }
   
   if (pChunkPkt == NULL)
   {
      printf("testAssemblyChunkPacket: failure at line #%d\n", __LINE__);
      return false;
   }
   
   if (pChunkPkt->dataSize() != sizeof(Pattern))
   {
      printf("testAssemblyChunkPacket: failure at line #%d\n", __LINE__);
      return false;
   }
   
   for (ui32 l_nInd = 0; l_nInd < sizeof(Pattern); l_nInd++)
   {
      if (pChunkPkt->data()[l_nInd] != Pattern[l_nInd])
      {
         printf("testAssemblyChunkPacket: failure at line #%d\n", __LINE__);
         return false;
      }
   }
   
   delete[] l_pPackedPkt; l_pPackedPkt = NULL;
   delete pChunkPkt; pChunkPkt = NULL;
   delete pInstr; pInstr = NULL;
   
   return true;
}

//------------------------------------------------------------------------------
bool testAssemblySegment()
{
   RsyncAssemblyInstr* pInstr = NULL;
   AssemblySegmentPacket* pSegment = NULL;
   
   char* l_pPackedPkt = NULL;
   ui32  l_nPktSize = 0;
   
   ui32  txSegmentId = 32;
   ui32  rxSegmentId = 0;
   
   pSegment = new AssemblySegmentPacket(txSegmentId);
   pInstr = new RsyncAssemblyInstr(pSegment);
   
   if (pInstr->type() != RsyncAssemblyInstr::SegmentType)
   {
      printf("testAssemblySegment: failure at line #%d\n", __LINE__);
      return false;
   }
   
   if (!pInstr->pack((void**)&l_pPackedPkt, l_nPktSize))
   {
      printf("testAssemblySegment: failure at line #%d\n", __LINE__);
      return false;
   }
   
   if (l_pPackedPkt == NULL)
   {
      printf("testAssemblySegment: failure at line #%d\n", __LINE__);
      return false;
   }
   
   if (l_nPktSize != (sizeof(RsyncAssemblyInstr::Data) +
                      sizeof(AssemblySegmentPacket::Data)))
   {
      printf("testAssemblySegment: failure at line #%d\n", __LINE__);
      return false;
   }
   
   delete pInstr; pInstr = NULL;
   delete pSegment; pSegment = NULL;
   
   pInstr = new RsyncAssemblyInstr();
   
   if (!pInstr->unpack(l_pPackedPkt, l_nPktSize))
   {
      printf("testAssemblySegment: failure at line #%d\n", __LINE__);
      return false;
   }
   
   if (pInstr->type() != RsyncAssemblyInstr::SegmentType)
   {
      printf("testAssemblySegment: failure at line #%d\n", __LINE__);
      return false;
   }
   
   if (!pInstr->to(&pSegment))
   {
      printf("testAssemblySegment: failure at line #%d\n", __LINE__);
      return false;
   }
   
   if (pSegment == NULL)
   {
      printf("testAssemblySegment: failure at line #%d\n", __LINE__);
      return false;
   }
   
   if (!pSegment->getSegmentId(rxSegmentId))
   {
      printf("testAssemblySegment: failure at line #%d\n", __LINE__);
      return false;
   }
   
   if (rxSegmentId != txSegmentId)
   {
      printf("testAssemblySegment: failure at line #%d\n", __LINE__);
      return false;
   }
   
   delete[] l_pPackedPkt; l_pPackedPkt = NULL;
   delete pSegment; pSegment = NULL;
   delete pInstr; pInstr = NULL;
   
   return true;
}

//------------------------------------------------------------------------------
bool testRsyncSegmentReportHdr()
{
   RsyncSegmentReportPacket* pPacket = NULL;
   RsyncSegmentReportHdr* pHdr = NULL;
   
   char* l_pPackedPkt = NULL;
   ui32  l_nPktSize = 0;
   
   std::string txFilePath = "/root/test/data.ifo";
   ui32  txSegmentCount = 32;
   ui32  txSegmentSize = 16;
   
   pHdr = new RsyncSegmentReportHdr();
   
   pHdr->setFilePath(txFilePath);
   pHdr->setSegmentCount(txSegmentCount);
   pHdr->setSegmentSize(txSegmentSize);
   
   pPacket = new RsyncSegmentReportPacket(pHdr);
   
   if (pPacket->type() != RsyncSegmentReportPacket::HeaderType)
   {
      printf("testRsyncSegmentReportHdr: failure at line #%d\n", __LINE__);
      return false;
   }
   
   if (!pPacket->pack((void**)&l_pPackedPkt, l_nPktSize))
   {
      printf("testRsyncSegmentReportHdr: failure at line #%d\n", __LINE__);
      return false;
   }
   
   if (l_pPackedPkt == NULL)
   {
      printf("testRsyncSegmentReportHdr: failure at line #%d\n", __LINE__);
      return false;
   }
   
   if (l_nPktSize != (sizeof(RsyncSegmentReportPacket::Data) +
                      sizeof(RsyncSegmentReportHdr::Data)))
   {
      printf("testRsyncSegmentReportHdr: failure at line #%d\n", __LINE__);
      return false;
   }
   
   delete pHdr; pHdr = NULL;
   delete pPacket; pPacket = NULL;
   
   pPacket = new RsyncSegmentReportPacket();
   
   if (!pPacket->unpack(l_pPackedPkt, l_nPktSize))
   {
      printf("testRsyncSegmentReportHdr: failure at line #%d\n", __LINE__);
      return false;
   }
   
   if (pPacket->type() != RsyncSegmentReportPacket::HeaderType)
   {
      printf("testRsyncSegmentReportHdr: failure at line #%d\n", __LINE__);
      return false;
   }
   
   if (!pPacket->to(&pHdr))
   {
      printf("testRsyncSegmentReportHdr: failure at line #%d\n", __LINE__);
      return false;
   }
   
   if (pHdr == NULL)
   {
      printf("testRsyncSegmentReportHdr: failure at line #%d\n", __LINE__);
      return false;
   }
   
   std::string rxFilePath = "";
   ui32  rxSegmentCount = 0;
   ui32  rxSegmentSize = 0;
   if (!pHdr->getFilePath(rxFilePath))
   {
      printf("testAssemblySegment: failure at line #%d\n", __LINE__);
      return false;
   }
   
   if (!pHdr->getSegmentCount(rxSegmentCount))
   {
      printf("testRsyncSegmentReportHdr: failure at line #%d\n", __LINE__);
      return false;
   }
   
   if (!pHdr->getSegmentSize(rxSegmentSize))
   {
      printf("testRsyncSegmentReportHdr: failure at line #%d\n", __LINE__);
      return false;
   }
   
   if (rxFilePath != txFilePath)
   {
      printf("testRsyncSegmentReportHdr: failure at line #%d\n", __LINE__);
      return false;
   }
   
   if (rxSegmentCount != txSegmentCount)
   {
      printf("testRsyncSegmentReportHdr: failure at line #%d\n", __LINE__);
      return false;
   }
   
   if (rxSegmentSize != txSegmentSize)
   {
      printf("testRsyncSegmentReportHdr: failure at line #%d\n", __LINE__);
      return false;
   }
   
   delete[] l_pPackedPkt; l_pPackedPkt = NULL;
   delete pHdr; pHdr = NULL;
   delete pPacket; pPacket = NULL;
   
   return true;
}

//------------------------------------------------------------------------------
bool testRsyncSegmentPacket()
{
   static const ui8 Pattern[] = { 0x0F, 0x1E, 0x2D, 0x3C, 0x4B, 0x5A, 0x69,
                                 0x78, 0x87, 0x96, 0xA5, 0xB4, 0xC3, 0xD2,
                                 0xE1, 0xF0 };
   
   RsyncSegmentReportPacket* pPacket = NULL;
   RsyncSegmentPacket* pSegment = NULL;
   
   char* l_pPackedPkt = NULL;
   ui32  l_nPktSize = 0;
   
   ui32    txSegmentId = 6;
   i32     txWeakChk = 42583;
   Hash128 txStrongChk; char* pTxStrongChk = (char*)&txStrongChk;
   ui16    txSegSizeBytes = 16;
   
   memcpy(pTxStrongChk, Pattern, sizeof(Hash128));
   
   pSegment = new RsyncSegmentPacket();
   
   pSegment->setSegmentId(txSegmentId);
   pSegment->setWeak(txWeakChk);
   pSegment->setStrong(txStrongChk);
   pSegment->setSegmentSize(txSegSizeBytes);
   
   pPacket = new RsyncSegmentReportPacket(pSegment);
   
   if (pPacket->type() != RsyncSegmentReportPacket::SegmentType)
   {
      printf("testRsyncSegmentPacket: failure at line #%d\n", __LINE__);
      return false;
   }
   
   if (!pPacket->pack((void**)&l_pPackedPkt, l_nPktSize))
   {
      printf("testRsyncSegmentPacket: failure at line #%d\n", __LINE__);
      return false;
   }
   
   if (l_pPackedPkt == NULL)
   {
      printf("testRsyncSegmentPacket: failure at line #%d\n", __LINE__);
      return false;
   }
   
   if (l_nPktSize != (sizeof(RsyncSegmentReportPacket::Data) +
                      sizeof(RsyncSegmentPacket::Data)))
   {
      printf("testRsyncSegmentPacket: failure at line #%d\n", __LINE__);
      return false;
   }
   
   delete pSegment; pSegment = NULL;
   delete pPacket; pPacket = NULL;
   
   pPacket = new RsyncSegmentReportPacket();
   
   if (!pPacket->unpack(l_pPackedPkt, l_nPktSize))
   {
      printf("testRsyncSegmentPacket: failure at line #%d\n", __LINE__);
      return false;
   }
   
   if (pPacket->type() != RsyncSegmentReportPacket::SegmentType)
   {
      printf("testRsyncSegmentPacket: failure at line #%d\n", __LINE__);
      return false;
   }
   
   if (!pPacket->to(&pSegment))
   {
      printf("testRsyncSegmentPacket: failure at line #%d\n", __LINE__);
      return false;
   }
   
   if (pSegment == NULL)
   {
      printf("testRsyncSegmentPacket: failure at line #%d\n", __LINE__);
      return false;
   }
   
   ui32    rxSegmentId;
   i32     rxWeakChk;
   Hash128 rxStrongChk; char* pRxStrongChk = (char*)&rxStrongChk;
   ui16    rxSegSizeBytes;
   
   if (!pSegment->getSegmentId(rxSegmentId))
   {
      printf("testRsyncSegmentPacket: failure at line #%d\n", __LINE__);
      return false;
   }
   
   if (!pSegment->getWeak(rxWeakChk))
   {
      printf("testRsyncSegmentPacket: failure at line #%d\n", __LINE__);
      return false;
   }
   
   if (!pSegment->getStrong(rxStrongChk))
   {
      printf("testRsyncSegmentPacket: failure at line #%d\n", __LINE__);
      return false;
   }
   
   if (!pSegment->getSegmentSize(rxSegSizeBytes))
   {
      printf("testRsyncSegmentPacket: failure at line #%d\n", __LINE__);
      return false;
   }
   
   if (rxSegmentId != txSegmentId)
   {
      printf("testRsyncSegmentPacket: failure at line #%d\n", __LINE__);
      return false;
   }
   
   if (rxWeakChk != txWeakChk)
   {
      printf("testRsyncSegmentPacket: failure at line #%d\n", __LINE__);
      return false;
   }
   
   if (rxSegSizeBytes != txSegSizeBytes)
   {
      printf("testRsyncSegmentPacket: failure at line #%d\n", __LINE__);
      return false;
   }
   
   for (ui32 l_nOffs = 0; l_nOffs < sizeof(Pattern); l_nOffs++)
   {
      if (pTxStrongChk[l_nOffs] != pRxStrongChk[l_nOffs])
      {
         printf("testRsyncSegmentPacket: failure at line #%d\n", __LINE__);
         return false;
      }
   }
   
   delete[] l_pPackedPkt; l_pPackedPkt = NULL;
   delete pSegment; pSegment = NULL;
   delete pPacket; pPacket = NULL;
   
   return true;
}
