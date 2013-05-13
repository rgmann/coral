#include "RsyncSegmentTable.h"

//------------------------------------------------------------------------------
RsyncSegmentTable::RsyncSegmentTable()
{
   //memset(&m_Header, 0, sizeof(RsyncReportHeader));
   m_pHeader = NULL;
}

//------------------------------------------------------------------------------
//RsyncSegmentTable::~RsyncSegmentTable()
//{
//   // The hash is the persistent container for received header/segments streams.
//   // When the hash is destroyed, it de-allocates all associated memory
//   std::vector<RsyncPackedSeg*>::iterator l_iSegIt;
//   for (l_iSegIt = m_vSegVec.begin(); l_iSegIt < m_vSegVec.end(); ++l_iSegIt)
//   {
//      delete *l_iSegIt;
//   }
//}
RsyncSegmentTable::~RsyncSegmentTable()
{
   // The hash is the persistent container for received header/segments streams.
   // When the hash is destroyed, it de-allocates all associated memory
   std::vector<RsyncSegmentPacket*>::iterator l_iSegIt;
   for (l_iSegIt = m_vSegVec.begin(); l_iSegIt < m_vSegVec.end(); ++l_iSegIt)
   {
      delete *l_iSegIt;
   }
}

//------------------------------------------------------------------------------
//void RsyncSegmentTable::setHeader(RsyncReportHeader* pHeader)
//{
//   if (pHeader)
//   {
//      memcpy(&m_Header, pHeader, sizeof(RsyncReportHeader));
//   }
//}
void RsyncSegmentTable::setHeader(RsyncSegmentReportHdr* pHeader)
{
   m_pHeader = pHeader;
}

//------------------------------------------------------------------------------
//bool RsyncSegmentTable::getHeader(RsyncReportHeader* pHeader) const
//{
//   if (pHeader)
//   {
//      memcpy(pHeader, &m_Header, sizeof(RsyncReportHeader));
//      return true;
//   }
//   
//   return false;
//}
bool RsyncSegmentTable::getHeader(RsyncSegmentReportHdr* pHeader) const
{
   if (m_pHeader)
   {
      pHeader = m_pHeader;
      return true;
   }

   return false;
}

//------------------------------------------------------------------------------
//bool RsyncSegmentTable::addSegment(RsyncPackedSeg* pSegment)
//{
//   //RsyncPackedSeg l_Segment;
//   RsyncPackedSeg* l_pSegment = NULL;
//   
//   if (pSegment == NULL)
//   {
//      return false;
//   }
//   
//   l_pSegment = new RsyncPackedSeg();
//   if (l_pSegment == NULL)
//   {
//      return false;
//   }
//   
//   memcpy(l_pSegment, pSegment, sizeof(RsyncPackedSeg));
//   m_vSegVec.push_back(l_pSegment);
//   
//   return m_hHashTable.add(pSegment->weakChecksum, l_pSegment, true);
//}
bool RsyncSegmentTable::addSegment(RsyncSegmentReportPacket* pPacket)
{
   bool  l_bSuccess = false;
   i32   l_nWeakChecksum = 0;
   RsyncSegmentPacket* l_pSegment = NULL;
   
   if (pPacket == NULL)
   {
      printf("RsyncSegmentTable::addSegment: NULL packet\n");
      return false;
   }
   
   if (pPacket->type() == RsyncSegmentReportPacket::SegmentType)
   {
      l_pSegment = reinterpret_cast<RsyncSegmentPacket*>(pPacket);
      
      m_vSegVec.push_back(l_pSegment);
      l_pSegment->getWeak(l_nWeakChecksum);
      
      l_bSuccess = m_hHashTable.add(l_nWeakChecksum, l_pSegment, true);
      
      if (l_bSuccess)
      {
         printf("RsyncSegmentTable::addSegment: SegmentType 0x%08X\n",
                l_nWeakChecksum);
      }
   }
   else if (pPacket->type() == RsyncSegmentReportPacket::HeaderType)
   {
      m_pHeader = reinterpret_cast<RsyncSegmentReportHdr*>(pPacket);
      
      printf("RsyncSegmentTable::addSegment: HeaderType\n");
      
      l_bSuccess = true;
   }
   else
   {
      printf("RsyncSegmentTable::addSegment: unregonized type #%u\n",
             pPacket->type());
   }
   
   return l_bSuccess;
}

//------------------------------------------------------------------------------
bool RsyncSegmentTable::find(RsyncSegment &segment,
                             RsyncSegmentPacket* pPacket)
{
   std::vector<RsyncSegmentPacket*> l_vSegVec;
   std::vector<RsyncSegmentPacket*>::iterator l_iSegIt;
   Adler32Checksum   weaksum;
   Md5Hash           strongChecksum;
   Hash128           strongHash;
   Hash128           l_itStrongHash;
   bool              l_bFound = false;
   
   segment.getWeak(weaksum);
   l_vSegVec = m_hHashTable.get(weaksum.s);
   
   if (l_vSegVec.empty())
   {
      return false;
   }
   
   segment.getStrong(strongChecksum);
   strongChecksum.get(&strongHash);

   l_bFound = false;
   for (l_iSegIt = l_vSegVec.begin(); l_iSegIt < l_vSegVec.end(); ++l_iSegIt)
   {
      (*l_iSegIt)->getStrong(l_itStrongHash);
      if (CompareStrong(&strongHash, &l_itStrongHash))
      {
         pPacket = *l_iSegIt;
         l_bFound = true;
         break;
      }
   }
   
   return l_bFound;
}

//------------------------------------------------------------------------------
//bool RsyncSegmentTable::find(RsyncSegment &segment, std::ifstream &ifs,
//                             RsyncPackedSeg &packedSegment)
//{
//   std::vector<RsyncPackedSeg*> l_vSegVec;
//   std::vector<RsyncPackedSeg*>::iterator l_iSegIt;
//   Adler32Checksum   weaksum;
//   Md5Hash           strongChecksum;
//   Hash128           strongHash;
//   bool              l_bFound = false;
//   
//   segment.getWeak(weaksum);
//   l_vSegVec = m_hHashTable.get(weaksum.s);
//   
//   if (l_vSegVec.empty())
//   {
//      return false;
//   }
//   
//   segment.getStrong(ifs, strongChecksum);
//   strongChecksum.get(&strongHash);
//   
//   l_bFound = false;
//   for (l_iSegIt = l_vSegVec.begin(); l_iSegIt < l_vSegVec.end(); ++l_iSegIt)
//   {
//      if (CompareStrong(&strongHash, &(*l_iSegIt)->strongChecksum))
//      {
//         memcpy(&packedSegment, *l_iSegIt, sizeof(RsyncPackedSeg));
//         l_bFound = true;
//         break;
//      }
//   }
//   
//   return l_bFound;
//}
bool RsyncSegmentTable::find(RsyncSegment &segment, std::ifstream &ifs,
                             RsyncSegmentPacket* pPacket)
{
   std::vector<RsyncSegmentPacket*> l_vSegVec;
   std::vector<RsyncSegmentPacket*>::iterator l_iSegIt;
   Adler32Checksum   weaksum;
   Md5Hash           strongChecksum;
   Hash128           strongHash;
   Hash128           l_itStrongHash;
   bool              l_bFound = false;
   
   segment.getWeak(weaksum);
   l_vSegVec = m_hHashTable.get(weaksum.s);
   
   if (l_vSegVec.empty())
   {
      return false;
   }
   
   segment.getStrong(ifs, strongChecksum);
   strongChecksum.get(&strongHash);
   
   l_bFound = false;
   for (l_iSegIt = l_vSegVec.begin(); l_iSegIt < l_vSegVec.end(); ++l_iSegIt)
   {
      (*l_iSegIt)->getStrong(l_itStrongHash);
      if (CompareStrong(&strongHash, &l_itStrongHash))
      {
         pPacket = *l_iSegIt;
         l_bFound = true;
         break;
      }
   }
   
   return l_bFound;
}

//------------------------------------------------------------------------------
//bool RsyncSegmentTable::isComplete()
//{
//   printf("isComplete: header.count=%u, table.count=%u\n",
//          m_Header.segmentCount, m_hHashTable.count());
//   return (m_Header.segmentCount == m_hHashTable.count());
//}
bool RsyncSegmentTable::isComplete()
{
   ui32 l_nExpectedSegCount = 0;
   
   m_pHeader->getSegmentCount(l_nExpectedSegCount);
   printf("isComplete: header.count=%u, table.count=%u\n",
          l_nExpectedSegCount, m_hHashTable.count());
   return (l_nExpectedSegCount == m_hHashTable.count());
}

//------------------------------------------------------------------------------
bool RsyncSegmentTable::CompareStrong(const Hash128* pStrongA,
                                      const Hash128* pStrongB)
{
   if (pStrongA == NULL || pStrongB == NULL)
   {
      return false;
   }
   
   for (unsigned int l_nIdx = 0; l_nIdx < sizeof(Hash128); l_nIdx++)
   {
      if (pStrongA->b[l_nIdx] != pStrongB->b[l_nIdx])
      {
         return false;
      }
   }
   
   return true;
}
