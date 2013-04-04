#include "RsyncSegment.h"
#include <string.h>
#include <stdio.h>

static const unsigned int RsyncSegmentMarker = 0xA5A5A5A5;

void PrintSegRsync(unsigned char* pBlock, int nBytes)
{
   for (int ind = 0; ind < nBytes; ind++)
      printf("%d ", *(pBlock + ind));
   printf("\n");
}

//------------------------------------------------------------------------------
RsyncSegment::RsyncSegment()
{
   m_pBlock = NULL;
}

//------------------------------------------------------------------------------
RsyncSegment::~RsyncSegment()
{
   if (m_pBlock)
   {
      delete[] m_pBlock;
      m_pBlock = NULL;
   }
}

//------------------------------------------------------------------------------
RsyncSegment* RsyncSegment::Create(/*const*/ unsigned char* pData,
                                   unsigned int nOffsetBytes,
                                   unsigned int nBlockSizeBytes,
                                   RsyncSegId   nId,
                                   const Adler32Checksum *pPrev,
                                   bool   bComputeStrong)
{
   RsyncSegment*  l_pSegment = NULL;
   
   l_pSegment = new RsyncSegment();
   if (l_pSegment == NULL)
   {
      return NULL;
   }
   
   l_pSegment->m_nBlockSizeBytes = nBlockSizeBytes;
   l_pSegment->m_nOffset         = nOffsetBytes;
   l_pSegment->m_nSegId = nId;
   
//   l_pSegment->m_pBlock = new unsigned char[nBlockSizeBytes];
//   if (l_pSegment->m_pBlock == NULL)
//   {
//      delete l_pSegment;
//      l_pSegment = NULL;
//      return NULL;
//   }
//   
//   memcpy(l_pSegment->m_pBlock, 
//          pData, 
//          l_pSegment->m_nBlockSizeBytes);
   
   if (pPrev != NULL)
   {
      RollWeak(l_pSegment->m_WeakChk, 
               pData, nOffsetBytes, 
               nBlockSizeBytes, 
               *pPrev);
   }
   else
   {
      ComputeWeak(l_pSegment->m_WeakChk, 
                  pData, nOffsetBytes, 
                  nBlockSizeBytes);
   }
   
   if (bComputeStrong)
   {
      l_pSegment->m_StrongChk.hashify(pData, nBlockSizeBytes);
   }

   return l_pSegment;
}

//------------------------------------------------------------------------------
void RsyncSegment::getWeak(Adler32Checksum &weak)
{
   memcpy(&weak, &m_WeakChk, sizeof(Adler32Checksum));
}

//------------------------------------------------------------------------------
void RsyncSegment::getStrong(Md5Hash &strong)
{
   if (!m_StrongChk.isValid() && m_pBlock)
   {
      m_StrongChk.hashify(m_pBlock, m_nBlockSizeBytes);
   }
   
   strong = m_StrongChk;
}

//------------------------------------------------------------------------------
bool RsyncSegment::getStrong(std::ifstream &ifs, Md5Hash &strong)
{
   unsigned char *l_pBuffer = NULL;
   bool  l_bSuccess = true;
   
   if (!m_StrongChk.isValid())
   {
      l_bSuccess = false;
      
      ifs.seekg(m_nOffset, ifs.beg);
      if (!ifs.good())
      {
         return false;
      }
      
      l_pBuffer = new unsigned char[m_nBlockSizeBytes];
      if (l_pBuffer == NULL)
      {
         return false;
      }
      
      ifs.read((char*)l_pBuffer, m_nBlockSizeBytes);
      if (ifs.good())
      {
         m_StrongChk.hashify(l_pBuffer, m_nBlockSizeBytes);
         l_bSuccess = true;
      }
      
      delete[] l_pBuffer;
      l_pBuffer = NULL;
   }
   
   strong = m_StrongChk;
   
   return true;
}

//------------------------------------------------------------------------------
bool RsyncSegment::getData(unsigned char **pBlock, unsigned int &nSizeBytes)
{
   if (m_pBlock == NULL || *pBlock != NULL)
   {
      return false;
   }
   
   nSizeBytes = m_nBlockSizeBytes;
   *pBlock = new unsigned char[m_nBlockSizeBytes];
   if (*pBlock == NULL)
   {
      return false;
   }
   
   memcpy(*pBlock, m_pBlock, m_nBlockSizeBytes);
   
   return true;
}

//------------------------------------------------------------------------------
bool RsyncSegment::getData(std::ifstream &ifs,
                           unsigned char **pBlock,
                           unsigned int &nSizeBytes)
{
   //if (m_pBlock == NULL || *pBlock != NULL)
   if (*pBlock != NULL)
   {
      return false;
   }
   
   nSizeBytes = m_nBlockSizeBytes;
   *pBlock = new unsigned char[m_nBlockSizeBytes];
   
   if (*pBlock == NULL)
   {
      return false;
   }
   
   //memcpy(*pBlock, m_pBlock, m_nBlockSizeBytes);
   
   ifs.seekg(m_nOffset, ifs.beg);
   ifs.read((char*)*pBlock, m_nBlockSizeBytes);
   
   return ifs.good();
}

//------------------------------------------------------------------------------
unsigned int RsyncSegment::getBlockSize()
{
   return m_nBlockSizeBytes;
}

//------------------------------------------------------------------------------
unsigned char* RsyncSegment::getBlockPtr()
{
   return m_pBlock;
}

//------------------------------------------------------------------------------
RsyncSegId  RsyncSegment::getId()
{
   return m_nSegId;
}

//------------------------------------------------------------------------------
bool RsyncSegment::pack(RsyncPackedSeg *pSeg)
{
   if (pSeg == NULL || !m_StrongChk.isValid())
   {
      return false;
   }
   
   //Md5Hash l_strongHash;
   
   // Copy header into destination
   pSeg->segmentId        = getId();
   pSeg->weakChecksum      = m_WeakChk.s;
   
//   getStrong(l_strongHash);
//   l_strongHash.get(&pSeg->strongChecksum);
   m_StrongChk.get(&pSeg->strongChecksum);
   
   pSeg->nSegSizeBytes = getBlockSize();
   
   return true;
}

//------------------------------------------------------------------------------
void RsyncSegment::ComputeWeak(Adler32Checksum &next, 
                               const unsigned char* pData,
                               unsigned int nOffset,
                               unsigned int nBlockSizeBytes)
{
   // 
   unsigned int l_nDataOffs = 0;
   
   next.k = nOffset;
   next.l = (nOffset + nBlockSizeBytes - 1);
   
   next.a = 0;
   for (unsigned int l_nIter = next.k; l_nIter <= next.l; l_nIter++)
   {
      l_nDataOffs = l_nIter - nOffset;
      next.a += (int)*(pData + l_nDataOffs);
   }
   next.a = next.a % M;
   
   next.b = 0;
   for (unsigned int l_nIter = next.k; l_nIter <= next.l; l_nIter++)
   {
      l_nDataOffs = l_nIter - nOffset;
      next.b += (next.l - l_nIter + 1) * (*(pData + l_nDataOffs));
   }
   next.b = next.b % M;
   
   next.s = next.a + M * next.b;
   
   l_nDataOffs = next.k - nOffset;
   next.x_k = *(pData + l_nDataOffs);
   
   l_nDataOffs = next.l - nOffset;
   next.x_l = *(pData + l_nDataOffs);
   
   printf("CW: k = %d, l = %d, a = %d, b = %d, s = %x, (%c,%c)\n", 
          next.k, next.l, next.a, next.b, next.s, next.x_k, next.x_l);
}

//------------------------------------------------------------------------------
void RsyncSegment::RollWeak(Adler32Checksum &next, 
                            const unsigned char* pData,
                            unsigned int nOffset,
                            unsigned int nBlockSizeBytes, 
                            const Adler32Checksum &prev)
{
   //
   unsigned int l_nDataOffs = 0;
   
   next.k = prev.k + 1;
   next.l = prev.l + 1;
   
   // Check to see if the this is a shorter block:
   // "The last block may be shorter than S bytes."
//   if (next.l >= (nOffset + nBlockSizeBytes))
//   {
//      next.l = (nOffset + nBlockSizeBytes - 1);
//   }
   
   l_nDataOffs = next.l - nOffset;
   next.a = (prev.a - prev.x_k + *(pData + l_nDataOffs)) % M;
   next.b = (prev.b - (prev.l - prev.k + 1) * prev.x_k + next.a) % M;
   
   next.s = next.a + M * next.b;
   
   l_nDataOffs = next.k - nOffset;
   next.x_k = *(pData + l_nDataOffs);
   
   l_nDataOffs = next.l - nOffset;
   next.x_l = *(pData + l_nDataOffs);
   
   printf("RW: k = %d, l = %d, a = %d, b = %d, s = %x, (%c,%c)\n", 
          next.k, next.l, next.a, next.b, next.s, next.x_k, next.x_l);
}
