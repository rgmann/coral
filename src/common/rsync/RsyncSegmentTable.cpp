#include "RsyncSegmentTable.h"

//------------------------------------------------------------------------------
RsyncSegmentTable::RsyncSegmentTable()
{
   memset(&m_Header, 0, sizeof(RsyncReportHeader));
}

//------------------------------------------------------------------------------
RsyncSegmentTable::~RsyncSegmentTable()
{
   // The hash is the persistent container for received header/segments streams.
   // When the hash is destroyed, it de-allocates all associated memory
   std::vector<RsyncPackedSeg*>::iterator l_iSegIt;
   for (l_iSegIt = m_vSegVec.begin(); l_iSegIt < m_vSegVec.end(); ++l_iSegIt)
   {
      delete *l_iSegIt;
   }
}

//------------------------------------------------------------------------------
void RsyncSegmentTable::setHeader(RsyncReportHeader* pHeader)
{
   if (pHeader)
   {
      memcpy(&m_Header, pHeader, sizeof(RsyncReportHeader));
   }
}

//------------------------------------------------------------------------------
bool RsyncSegmentTable::getHeader(RsyncReportHeader* pHeader) const
{
   if (pHeader)
   {
      memcpy(pHeader, &m_Header, sizeof(RsyncReportHeader));
      return true;
   }
   
   return false;
}

//------------------------------------------------------------------------------
bool RsyncSegmentTable::addSegment(RsyncPackedSeg* pSegment)
{
   //RsyncPackedSeg l_Segment;
   RsyncPackedSeg* l_pSegment = NULL;
   
   if (pSegment == NULL)
   {
      return false;
   }
   
   l_pSegment = new RsyncPackedSeg();
   if (l_pSegment == NULL)
   {
      return false;
   }
   
   //memcpy(&l_Segment, pSegment, sizeof(RsyncPackedSeg));
   memcpy(l_pSegment, pSegment, sizeof(RsyncPackedSeg));
   m_vSegVec.push_back(l_pSegment);
   
//   return m_hHashTable.add(pSegment->weakChecksum, pSegment);
   //return m_hHashTable.add(pSegment->weakChecksum, l_Segment);
   //return m_hHashTable.add(pSegment->weakChecksum, l_Segment, true);
   return m_hHashTable.add(pSegment->weakChecksum, l_pSegment, true);
}

//------------------------------------------------------------------------------
bool RsyncSegmentTable::find(RsyncSegment &segment,
                             RsyncPackedSeg &packedSegment)
{
   std::vector<RsyncPackedSeg*> l_vSegVec;
   std::vector<RsyncPackedSeg*>::iterator l_iSegIt;
   Adler32Checksum   weaksum;
   Md5Hash           strongChecksum;
   Hash128           strongHash;
   bool              l_bFound = false;
   
   segment.getWeak(weaksum);
   //l_bFound = m_hHashTable.get(weaksum.s, packedSegment);
   //l_bFound = m_hHashTable.get(weaksum.s, l_vSegVec);
   l_vSegVec = m_hHashTable.get(weaksum.s);
   
   //if (!l_bFound)
   if (l_vSegVec.empty())
   {
      return false;
   }
   
   segment.getStrong(strongChecksum);
   strongChecksum.get(&strongHash);
//
//   if (!CompareStrong(&strongHash, &packedSegment.strongChecksum))
//   {
//      return false;
//   }
   l_bFound = false;
   for (l_iSegIt = l_vSegVec.begin(); l_iSegIt < l_vSegVec.end(); ++l_iSegIt)
   {
      //if (CompareStrong(&strongHash, &l_iSegIt->strongChecksum))
      if (CompareStrong(&strongHash, &(*l_iSegIt)->strongChecksum))
      {
         memcpy(&packedSegment, *l_iSegIt, sizeof(RsyncPackedSeg));
         l_bFound = true;
         break;
      }
   }
   
   //return true;
   return l_bFound;
}

//------------------------------------------------------------------------------
bool RsyncSegmentTable::find(RsyncSegment &segment, std::ifstream &ifs,
                             RsyncPackedSeg &packedSegment)
{
   std::vector<RsyncPackedSeg*> l_vSegVec;
   std::vector<RsyncPackedSeg*>::iterator l_iSegIt;
   Adler32Checksum   weaksum;
   Md5Hash           strongChecksum;
   Hash128           strongHash;
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
      if (CompareStrong(&strongHash, &(*l_iSegIt)->strongChecksum))
      {
         memcpy(&packedSegment, *l_iSegIt, sizeof(RsyncPackedSeg));
         l_bFound = true;
         break;
      }
   }
   
   return l_bFound;
}

//------------------------------------------------------------------------------
bool RsyncSegmentTable::isComplete()
{
   printf("isComplete: header.count=%u, table.count=%u\n",
          m_Header.segmentCount, m_hHashTable.count());
   return (m_Header.segmentCount == m_hHashTable.count());
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
