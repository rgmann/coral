#include "RsyncSegmentReport.h"
#include "RsyncHelper.h"
#include "FileSystemHelper.h"
#include <stdlib.h>
#include <stdio.h>

//------------------------------------------------------------------------------
RsyncSegmentReport::RsyncSegmentReport()
{
   //resetSegIndex();
}

//------------------------------------------------------------------------------
RsyncSegmentReport::~RsyncSegmentReport()
{
   
}

//------------------------------------------------------------------------------
RsyncSegmentReport* RsyncSegmentReport::Build(const std::string &rootPath,
                                              const std::string &relPath,
                                              unsigned int nBlockSizeBytes)
{
   bool l_bSegStatus = false;
   RsyncSegmentReport* l_pSegReport = NULL;
   
   l_pSegReport = new RsyncSegmentReport();
   if (l_pSegReport == NULL)
   {
      return NULL;
   }
   
   l_pSegReport->m_sRootPath = rootPath;
   l_pSegReport->m_sFilename = relPath;
   l_pSegReport->m_nBlockSizeBytes = nBlockSizeBytes;
   
   l_bSegStatus = RsyncHelper::SegmentFile(rootPath,
                                           relPath,
                                           l_pSegReport->m_vSegVec,
                                           nBlockSizeBytes,
                                           nBlockSizeBytes,
                                           RsyncHelper::DoNotRoll,
                                           RsyncHelper::DoStrongComp);
   if (!l_bSegStatus)
   {
      delete l_pSegReport;
      l_pSegReport = NULL;
      return NULL;
   }
   
   l_pSegReport->m_iSegIt = l_pSegReport->m_vSegVec.begin();
   
   return l_pSegReport;
}

//------------------------------------------------------------------------------
void RsyncSegmentReport::cancel()
{
   m_bCancelled = true;
}

//------------------------------------------------------------------------------
bool RsyncSegmentReport::cancelled() const
{
   return m_bCancelled;
}

//------------------------------------------------------------------------------
std::string RsyncSegmentReport::fullPath() const
{
   return FileSystemHelper::Concat(m_sRootPath, m_sFilename);
}

//------------------------------------------------------------------------------
RsyncSegVec& RsyncSegmentReport::segments()
{
   return m_vSegVec;
}

//------------------------------------------------------------------------------
bool RsyncSegmentReport::header(RsyncReportHeader* pHeader) const
{
   if (pHeader == NULL)
   {
      return false;
   }
   
   if (m_sFilename.length() > RsyncMaxPathLength)
   {
      return false;
   }
   
   pHeader->segmentCount = m_vSegVec.size();
   pHeader->segmentSizeBytes = m_nBlockSizeBytes;
   strncpy(pHeader->fullFileName, m_sFilename.c_str(), m_sFilename.length());
   
   return true;
}

//------------------------------------------------------------------------------
void RsyncSegmentReport::resetSegIndex()
{
   m_iSegIt = m_vSegVec.begin();
}

//------------------------------------------------------------------------------
bool RsyncSegmentReport::nextPackedSeg(RsyncPackedSeg* pSegment)
{
   bool l_bSuccess = false;
   
   if (m_iSegIt < m_vSegVec.end())
   {
      (*m_iSegIt)->pack(pSegment);
      m_iSegIt++;
      
      l_bSuccess = true;
   }
   
   return l_bSuccess;
}

//------------------------------------------------------------------------------
void RsyncSegmentReport::destroy()
{
   m_iSegIt = m_vSegVec.begin();
   
   for (; m_iSegIt < m_vSegVec.end(); ++m_iSegIt)
   {
      delete *m_iSegIt;
      *m_iSegIt = NULL;
   }
   
   m_vSegVec.clear();
}

//------------------------------------------------------------------------------
void RsyncSegmentReport::PrintReportHeader(const RsyncReportHeader* pHdr)
{
   printf("RsyncReportHeader:\n");
   printf("  marker           = 0x%08X\n", pHdr->marker);
   printf("  fullFileName     = %s\n", pHdr->fullFileName);
   printf("  segmentCount     = %u\n", pHdr->segmentCount);
   printf("  segmentSizeBytes = %u\n", pHdr->segmentSizeBytes);
}
