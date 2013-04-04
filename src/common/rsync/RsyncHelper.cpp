#include <iostream>
#include <fstream>
#include <stdio.h>
#include "RsyncHelper.h"
#include "FileSystemHelper.h"
#include "CircularBuffer.h"

//------------------------------------------------------------------------------
bool RsyncHelper::SegmentFile(const std::string &rootPath,
                              const std::string &relPath,
                              std::vector<RsyncSegment*> &segments,
                              unsigned int   nSegLenBytes,
                              unsigned int   nStrideBytes,
                              WeakPolicy  weakPol,
                              StrongPolicy strongPol)
{
   //int l_nBytes = 0;
   //char buffer[256];
   char*  l_pFileBuffer = NULL;
   char*  l_pReadBuffer = NULL;
   char   l_cTempChar;
   RsyncSegId l_nSegId = 0;
   
   int l_nFileLen = 0;
   //int l_nCurrPos = 0;
   
   Adler32Checksum   oldweaksum;
   Md5Hash           tempStrongHash;
   CircularBuffer    l_circBuff;
   
   std::string    fullFilePath = "";
   std::ifstream  is;
   
   // Allocated the buffer to the segment length.
   if (!l_circBuff.allocate(nSegLenBytes))
   {
      return false;
   }
   
   l_pFileBuffer = new char[nSegLenBytes];
   if (l_pFileBuffer == NULL)
   {
      return false;
   }
   
   l_pReadBuffer = new char[nSegLenBytes];
   if (l_pReadBuffer == NULL)
   {
      delete[] l_pFileBuffer;
      l_pFileBuffer = NULL;
      return false;
   }
   
   fullFilePath = FileSystemHelper::Concat(rootPath, relPath);
   is.open(fullFilePath.c_str(), std::ios::binary);
   
   if (is.fail())
   {
      std::cout << "Failed to open " << fullFilePath << std::endl;
      return false;
   }
   
   // Get the file length
   is.seekg (0, is.end);
   l_nFileLen = is.tellg();
   is.seekg (0, is.beg);
      
   std::cout << fullFilePath << ": " << std::endl;
   
   for (int l_nOffs = 0; l_nOffs < l_nFileLen; l_nOffs++) // l_nBytes
   {
      if ((l_nOffs % nStrideBytes) == 0)
      {
         unsigned int l_nReadLen = 0;
         unsigned int l_nRemaining = 0;
         unsigned int l_nSegLen = 0;
         
         Adler32Checksum* pAdler = NULL;
         RsyncSegment* l_pSeg    = NULL;
         
         if ((weakPol == DoRoll) && (l_nOffs > 0))
         {
            pAdler = &oldweaksum;
         }

         l_nRemaining = (l_nFileLen - l_nOffs);
//         l_nSegLen = (l_nRemaining < nSegLenBytes) ?
//                     l_nRemaining : nSegLenBytes;
         l_nSegLen = nSegLenBytes;
         if (l_nRemaining < nSegLenBytes)
         {
            l_nSegLen = l_nRemaining;
         }
         
         // Read the data into the buffer.
         if (l_nOffs == 0)
         {
            l_nReadLen = (l_nRemaining < nSegLenBytes) ?
                           l_nRemaining : nSegLenBytes;
         }
         else
         {
            l_nReadLen = (l_nRemaining < nStrideBytes) ?
                           l_nRemaining : nStrideBytes;
         }
         
         printf("seg: rem=%u, segLen=%u, readLen=%u\n",
                l_nRemaining, l_nSegLen, l_nReadLen);
                  
         is.read(l_pFileBuffer, l_nReadLen);
         l_circBuff.write(l_pFileBuffer, l_nReadLen);
         
         // For partial segments, it is necessary to read bytes to get
         // to the correct peek position.
         if (l_nSegLen < nSegLenBytes)
         {
            for (unsigned int l_nPeekCount = 0;
                 l_nPeekCount < (nSegLenBytes - l_nReadLen);
                 l_nPeekCount++)
            {
               l_circBuff.read(&l_cTempChar, 1);
            }
         }
         
         l_circBuff.peek(l_pReadBuffer, l_nSegLen);
         
         l_pSeg = RsyncSegment::Create((unsigned char*)l_pReadBuffer,
                                       l_nOffs,
                                       l_nSegLen,
                                       l_nSegId++,
                                       pAdler,
                                       (strongPol == DoStrongComp));
         if (l_pSeg == NULL)
         {
            printf("RsyncHelper::SegmentFile: Failed to allocate segment #%u\n",
                   l_nSegId);
            return false;
         }
         
         // Add the segment to the segment list vector.
         segments.push_back(l_pSeg);
         
         // Get the weak checksum for the next iteration just in case
         // checksum rolling is enabled.
         l_pSeg->getWeak(oldweaksum);
         
//         if (strongPol == DoStrongComp)
//         {
//            // Force segment to compute its strong checksum
//            l_pSeg->getStrong(tempStrongHash);
//         }
      }
   }
   
   delete[] l_pFileBuffer;
   delete[] l_pReadBuffer;
   
   is.close();
   return true;
}