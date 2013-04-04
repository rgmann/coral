#ifndef RSYNC_SEGMENT_H
#define RSYNC_SEGMENT_H

#include <stddef.h>
#include <fstream>
#include "Md5Hash.h"
#include "RsyncCommon.h"

struct Adler32Checksum
{
   int s;
   int a;
   int b;
   
   unsigned int   k;
   unsigned char  x_k;
   
   unsigned int   l;
   unsigned char  x_l;
};

typedef unsigned int RsyncSegId;

struct RsyncPackedSeg
{
   static const unsigned int marker = 0xA5A5A5A5;
   RsyncSegId        segmentId;
   int               weakChecksum;
   Hash128           strongChecksum;
   unsigned short    nSegSizeBytes;
};


class RsyncSegment
{
public:
   
   ~RsyncSegment();
   
   static RsyncSegment* Create(/*const*/ unsigned char* pData,
                               unsigned int nOffsetBytes,
                               unsigned int nBlockSizeBytes,
                               RsyncSegId   nId,
                               const Adler32Checksum *pPrev = NULL,
                               bool   bComputeStrong = false);
   
   void           getWeak(Adler32Checksum &weak);
   
   void           getStrong(Md5Hash &strong);
   
   bool           getStrong(std::ifstream &ifs, Md5Hash &strong);
   
   /**
    * TODO: Instead of storing data, save offset into file.  This function opens
    * the file and reads from the offset.
    */
   bool           getData(unsigned char **pBlock, unsigned int &nSizeBytes);
   
   bool           getData(std::ifstream &ifs,
                          unsigned char **pBlock,
                          unsigned int &nSizeBytes);
   
   unsigned char* getBlockPtr();
   
   unsigned int   getBlockSize();
   
   RsyncSegId     getId();
   
   bool           pack(RsyncPackedSeg *pSeg);
   
private:
   
   RsyncSegment();
   
   static void ComputeWeak(Adler32Checksum &weak, const unsigned char* pData,
                           unsigned int nOffset, unsigned int nBlockSizeBytes);
   
   static void RollWeak(Adler32Checksum &next, const unsigned char* pData,
                        unsigned int nOffset, unsigned int nBlockSizeBytes,
                        const Adler32Checksum &prev);
   
private:
   
   static const unsigned int M = 0x00010000;
      
   unsigned char* m_pBlock;
   unsigned int   m_nOffset;
   unsigned int   m_nBlockSizeBytes;
   
   Adler32Checksum   m_WeakChk;
   Md5Hash           m_StrongChk;
   
   RsyncSegId        m_nSegId;
};

#endif // RSYNC_SEGMENT_H