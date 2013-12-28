#ifndef RSYNC_SEGMENT_REPORT_H
#define RSYNC_SEGMENT_REPORT_H

#include <vector>
#include <string>
#include "RsyncSegment.h"
#include "RsyncSegmentReportHdr.h"

//struct RsyncReportHeader
//{
//   static const unsigned int   marker = 0x1B6CA55A;
//   char           fullFileName[RsyncMaxPathLength];
//   unsigned int   segmentCount;
//   unsigned int   segmentSizeBytes;
//};

//class RsyncSegmentReportHdr;

typedef std::vector<RsyncSegment*> RsyncSegVec;
//typedef std::vector<RsyncSegmentReportPacket*> RsyncSegVec;

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
   
//   bool  header(RsyncReportHeader* pHeader) const;
   bool  header(RsyncSegmentReportHdr** pHeader) const;
   
   void  resetSegIndex();
//   bool  nextPackedSeg(RsyncPackedSeg* pSegment);
   bool  nextSegment(RsyncSegmentPacket** pPacket);
   
   void  destroy();
   
//   static void PrintReportHeader(const RsyncReportHeader* pHdr);
   static void PrintReportHeader(const RsyncSegmentReportHdr* pHdr);
   
private:
   
   std::string m_sRootPath;
   std::string m_sFilename;
   
   unsigned int m_nBlockSizeBytes;
   
   bool  m_bCancelled;
   
   RsyncSegVec m_vSegVec;
   RsyncSegVec::iterator m_iSegIt;
};

#endif // RSYNC_SEGMENT_REPORT_H