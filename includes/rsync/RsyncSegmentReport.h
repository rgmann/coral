#ifndef RSYNC_SEGMENT_REPORT_H
#define RSYNC_SEGMENT_REPORT_H

#include "RsyncSegment.h"
#include <vector>
#include <string>

struct RsyncReportHeader
{
   static const unsigned int   marker = 0x1B6CA55A;
   char           fullFileName[RsyncMaxPathLength];
   unsigned int   segmentCount;
   unsigned int   segmentSizeBytes;
};

typedef std::vector<RsyncSegment*> RsyncSegVec;

class RsyncSegmentReport
{
public:
   
   RsyncSegmentReport();
   
   ~RsyncSegmentReport();
   
   static RsyncSegmentReport* Build(const std::string &rootPath,
                                    const std::string &relPath,
                                    unsigned int nBlockSizeBytes);
   
   void cancel();
   bool cancelled() const;
   
   std::string fullPath() const;

   RsyncSegVec& segments();
   
   bool  header(RsyncReportHeader* pHeader) const;
   
   void  resetSegIndex();
   bool  nextPackedSeg(RsyncPackedSeg* pSegment);
   
   void  destroy();
   
   static void PrintReportHeader(const RsyncReportHeader* pHdr);
   
private:
   
   std::string m_sRootPath;
   std::string m_sFilename;
   
   unsigned int m_nBlockSizeBytes;
   
   bool  m_bCancelled;
   
   RsyncSegVec m_vSegVec;
   RsyncSegVec::iterator m_iSegIt;
};

#endif // RSYNC_SEGMENT_REPORT_H