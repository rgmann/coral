#ifndef RSYNC_SEGMENT_TABLE_H
#define RSYNC_SEGMENT_TABLE_H

//#include "RsyncSegmentPacket.h"
//#include "RsyncSegmentReportHdr.h"
#include "RsyncSegmentReportPacket.h"
#include "RsyncSegmentReport.h"
#include "SimpleHashTable.h"
#include "CryptoHashTypes.h"

class RsyncSegmentTable
{
public:
   
   RsyncSegmentTable();
   
   ~RsyncSegmentTable();
   
//   void setHeader(RsyncReportHeader* pHeader);
   void setHeader(RsyncSegmentReportHdr* pHeader);
   
//   bool getHeader(RsyncReportHeader* pHeader) const;
   RsyncSegmentReportHdr* getHeader() const;
   
//   bool addSegment(RsyncPackedSeg* pSegment);
   bool addSegment(RsyncSegmentReportPacket* pSegment);
   
//   bool find(RsyncSegment &segment, RsyncPackedSeg &packedSegment);
   bool find(RsyncSegment &segment, RsyncSegmentPacket* pPacket);
   
//   bool find(RsyncSegment &segment, std::ifstream &ifs,
//             RsyncPackedSeg &packedSegment);
   RsyncSegmentPacket* find(RsyncSegment &segment, std::ifstream &ifs);//,
             //RsyncSegmentPacket* pPacket);
   
   /**
    * Returns true if the segment count indicated in the header
    * matches the number of segments in the hash table.
    */
   bool isComplete();
   
private:
   
   RsyncSegmentTable(const RsyncSegmentTable& other);
   
   RsyncSegmentTable& operator= (const RsyncSegmentTable& other);
   
   static bool CompareStrong(const Hash128* pStrongA,
                             const Hash128* pStrongB);
   
private:
   
//   RsyncReportHeader m_Header;
   RsyncSegmentReportHdr* m_pHeader;
   
//   std::vector<RsyncPackedSeg*> m_vSegVec;
   std::vector<RsyncSegmentPacket*> m_vSegVec;
   //SimpleHashTable<16, RsyncPackedSeg*> m_hHashTable;
   SimpleHashTable<16, RsyncSegmentPacket*> m_hHashTable;
};

#endif // RSYNC_SEGMENT_TABLE_H