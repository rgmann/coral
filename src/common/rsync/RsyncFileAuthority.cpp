#include "RsyncFileAuthority.h"
#include "RsyncHelper.h"
#include "FileSystemHelper.h"
#include <iostream>
#include <fstream>

//------------------------------------------------------------------------------
RsyncFileAuthority::RsyncFileAuthority()
{
   m_iInstrSeqIt = m_vInstrSeq.begin();
}

//------------------------------------------------------------------------------
RsyncFileAuthority::~RsyncFileAuthority()
{
}

//------------------------------------------------------------------------------
void RsyncFileAuthority::setRoot(const std::string &root)
{
   m_sRootDir = root;
}

//------------------------------------------------------------------------------
bool RsyncFileAuthority::process(RsyncSegmentTable* pSegTable)
{
   bool l_bSuccess = false;
   unsigned int l_nBytesSinceMatch = 0;
   unsigned int l_nOffset = 0;
   unsigned int l_nSegNum = 0;
   
   RsyncSegVec l_vSegVec;
   RsyncSegVec::iterator l_iCurrentSeg;
   
   Adler32Checksum   weaksum;
   Md5Hash           strongsum;
   //Hash128           stronghash;
   std::string       relFilePath;
   std::string       fullFilePath;
   RsyncReportHeader reportHeader;
   
   std::ifstream l_fAuthFile;
   
   
   if (!pSegTable)
   {
      return false;
   }
   
   // First we must segment the local version of the file.
   if (!pSegTable->getHeader(&reportHeader))
   {
      return false;
   }
   
   relFilePath = std::string(reportHeader.fullFileName);
   fullFilePath = FileSystemHelper::Concat(m_sRootDir, relFilePath);
   
   std::cout << "RsyncFileAuthority::process: Full path " << fullFilePath
   << std::endl;
   
   l_bSuccess = RsyncHelper::SegmentFile(m_sRootDir,
                                         relFilePath,
                                         l_vSegVec,
                                         reportHeader.segmentSizeBytes,
                                         1,
                                         RsyncHelper::DoRoll,
                                         RsyncHelper::DeferStrongComp);
   
   // If not successful, return
   if (!l_bSuccess)
   {
      std::cout << "RsyncFileAuthority::process: SegmentFile failed\n";
      return false;
   }
   
   // Reopen the file for the duration of the process.
   l_fAuthFile.open(fullFilePath.c_str(), std::ios::binary);
   if (l_fAuthFile.fail())
   {
      std::cout << "RsyncFileAuthority::process: SegmentFile failed\n";
      return false;
   }
   
   // Create the starting marker
   createBeginMarker(relFilePath);
   
   printf("PROCESS: seg count = %d\n", l_vSegVec.size());
   
   // Perform the segment search process.
   l_iCurrentSeg = l_vSegVec.begin();
   while (l_iCurrentSeg < l_vSegVec.end())
   {
      RsyncPackedSeg l_PackedSeg;
      bool           l_bFound = false;
      
      // Get the weak checksum for the current segment.
      (*l_iCurrentSeg)->getWeak(weaksum);
      
      // Attempt to retrieve the segment
//      l_bFound = pSegTable->find(**l_iCurrentSeg, l_PackedSeg);
      l_bFound = pSegTable->find(**l_iCurrentSeg, l_fAuthFile, l_PackedSeg);
      
      if (l_bFound)
      {
         // If we have a match and the l_nBytesSinceMatch count is > 0,
         // then we have to send a chunk instruction.
         if (l_nBytesSinceMatch > 0)
         {
            createChunkInstruction(l_fAuthFile, l_nOffset, l_nBytesSinceMatch);
         }
         
         // If a segment was found, then create an instruction to tell the
         // assembler which segment to use.
         createSegmentInstruction(l_PackedSeg.segmentId);
         l_nOffset += (l_nBytesSinceMatch + l_PackedSeg.nSegSizeBytes);
         
         // We have to shift the iterator by the size of the segment.
         for (int l_nShifts = 0;
              l_nShifts < l_PackedSeg.nSegSizeBytes &&
              l_iCurrentSeg < l_vSegVec.end(); l_nShifts++)
         {
            l_iCurrentSeg++; l_nSegNum++;
         }
         
         printf("MATCH: bytesSinceMath=%u, offs=%u, seg=%u\n",
                l_nBytesSinceMatch, l_nOffset, l_nSegNum);
         l_nBytesSinceMatch = 0;
      }
      else
      {
         if (l_nBytesSinceMatch > RsyncMaxChunkSize)
         {
            // For big files, we would like to limit the size of segments.
            createChunkInstruction(l_fAuthFile, l_nOffset, l_nBytesSinceMatch);
            l_nOffset += l_nBytesSinceMatch;
            l_nBytesSinceMatch = 0;
         }
         l_nSegNum++; l_iCurrentSeg++;
         l_nBytesSinceMatch++;
         printf("NOMATCH: bytesSinceMath=%u, offs=%u, seg=%u\n",
                l_nBytesSinceMatch, l_nOffset, l_nSegNum);
      }
   }
   
   if (l_nBytesSinceMatch > 0)
   {
      // Gets increment once more on the last iteration of the loop even
      // though there are no more segments.  Therefore, we have to shift back
      // by one byte :(
      //l_nBytesSinceMatch--;
      
      // If we get here, the last comparison did not result in a match,
      // so we have to create one more chink instruction.
      createChunkInstruction(l_fAuthFile, l_nOffset, l_nBytesSinceMatch);
      printf("REMAIN: bytesSinceMath=%u, offs=%u, seg=%u\n",
             l_nBytesSinceMatch, l_nOffset, l_nSegNum);
   }
   
   l_fAuthFile.close();
   
   createEndMarker();
   
   // Start the instruction iterator at the beginning the instruction vector.
   m_iInstrSeqIt = m_vInstrSeq.begin();
   
   return true;
}

//------------------------------------------------------------------------------
bool RsyncFileAuthority::getNextInstruction(RsyncAssemblyInstr **pInst)
{
   if (m_iInstrSeqIt < m_vInstrSeq.end())
   {
      if (*m_iInstrSeqIt == NULL) {
         std::cout << "NULL pointer" << std::endl;
      }
      *pInst = *m_iInstrSeqIt;
      m_iInstrSeqIt++;
      
      return true;
   }
   
   return false;
}

//------------------------------------------------------------------------------
void RsyncFileAuthority::cleanup()
{
   m_iInstrSeqIt = m_vInstrSeq.begin();
   for (; m_iInstrSeqIt < m_vInstrSeq.end(); ++m_iInstrSeqIt)
   {
      delete (*m_iInstrSeqIt);
      (*m_iInstrSeqIt) = NULL;
   }
   
   m_vInstrSeq.clear();
}

//------------------------------------------------------------------------------
bool RsyncFileAuthority::createBeginMarker(const std::string &relFilePath)
{
   RsyncAssemblyInstr* l_pSegment = NULL;
   
   l_pSegment = new RsyncAssemblyInstr(RsyncAssemblyInstr::RsyncBeginMarker,
                                       relFilePath.length() + 1);
   
   if (l_pSegment == NULL)
   {
      return false;
   }
   
   strncpy((char*)l_pSegment->dataPtr(),
           relFilePath.c_str(),
           relFilePath.length() + 1);
   
   m_vInstrSeq.push_back(l_pSegment);
   
   return true;
}

//------------------------------------------------------------------------------
bool RsyncFileAuthority::createEndMarker()
{
   RsyncAssemblyInstr* l_pSegment = NULL;
   
   l_pSegment = new RsyncAssemblyInstr(RsyncAssemblyInstr::RsyncEndMarker);
   
   if (l_pSegment == NULL)
   {
      return false;
   }
   
   m_vInstrSeq.push_back(l_pSegment);
   
   return true;
}

//------------------------------------------------------------------------------
bool RsyncFileAuthority::createSegmentInstruction(RsyncSegId segmentId)
{
   RsyncAssemblyInstr* l_pSegment = NULL;
   
   l_pSegment = new RsyncAssemblyInstr(RsyncAssemblyInstr::RsyncSegmentType,
                                       segmentId);
   
   if (l_pSegment == NULL)
   {
      return false;
   }

   m_vInstrSeq.push_back(l_pSegment);
   
   return true;
}

//------------------------------------------------------------------------------
bool RsyncFileAuthority::createChunkInstruction(std::ifstream &file,
                                                 unsigned int nOffsetBytes,
                                                 unsigned int nSizeBytes)
{
   RsyncAssemblyInstr* l_pSegment = NULL;
   //RsyncAssemblyInstrHdr* l_pInstrHdr;
   
   // Save the current file position just in case so we can restore it later.
   std::streampos l_fStreamPos = file.tellg();
   
   l_pSegment = new RsyncAssemblyInstr(RsyncAssemblyInstr::RsyncChunkType,
                                       nSizeBytes);
   
   if (l_pSegment == NULL)
   {
      return false;
   }
//   printf("createChunkInstruction: size=%u, offs=%u\n",
//          nSizeBytes, nOffsetBytes);
   
   // Seek to correct offset.
   file.seekg(nOffsetBytes, std::fstream::beg);
   
   if (file.fail())
   {
      file.seekg(l_fStreamPos);
      return false;
   }
   
   file.read((char*)l_pSegment->dataPtr(), l_pSegment->dataSize());
   
   // Restore the original file position.
   file.seekg(l_fStreamPos);
   
   if (!file.fail())
   {
      // Add the instruction to the vector.
      m_vInstrSeq.push_back(l_pSegment);
      
      return true;
   }
   printf("createChunkInstruction: failed to add instr\n");
   
   return false;
}
