#include "SegmenterUnit.h"

//------------------------------------------------------------------------------
SegmenterUnit::SegmenterUnit(Queue<RsyncPacket*>& outQueue)
: WorkUnit(outQueue)
{
   m_filenameQueue.initialize(Queue<std::string>::InfiniteQueue);
}

//------------------------------------------------------------------------------
void SegmenterUnit::setRoot(const std::string& root)
{
   m_segmenter.setRoot(root);
}

//------------------------------------------------------------------------------
bool SegmenterUnit::work()
{
   std::string l_sFilename = "";
   ui32 l_nSegCount = 0;
   RsyncSegmentReport*  l_pReport = NULL;
   RsyncSegmentPacket*  l_pSegment = NULL;
   RsyncSegmentReportPacket* l_pSegRepPkt = NULL;
   RsyncSegmentReportHdr*     l_pReportHdr = NULL;
   RsyncPacket*               l_pPacket = NULL;
   
   // The SegmenterUnit only does work when a file is available.  When a file
   // is available, it processes the entire file at once.
   
   if (!m_filenameQueue.pop(l_sFilename, 100)) return false;
   
   // Process the file.
   if (!m_segmenter.process(l_sFilename))
   {
      printf("SegmenetUnit failed to segment %s\n", l_sFilename.c_str());
      return false;
   }
   
   // Push the header into the output queue.
   // Once processing is complete, we can retrieve the segmenation report.
   l_pReport = m_segmenter.getReport(l_sFilename);
   if (l_pReport == NULL)
   {
      printf("Failed to get Segmentation Report\n");
      return false;
   }
   
   if (!l_pReport->header(&l_pReportHdr))
   {
      printf("Failed to get Segmentation Report header\n");
      return false;
   }
   
   // Push the report header and packed segments to the hash table on the
   // server.
   l_pSegRepPkt = new RsyncSegmentReportPacket(l_pReportHdr);
   l_pPacket = new RsyncPacket(l_pSegRepPkt);
   pushOutputPacket(l_pPacket);
   
   // The header packet is not longer needed, so delete it.
   delete l_pSegRepPkt;
   l_pSegRepPkt = NULL;

   // Push all segments into the output queue.
   while (l_pReport->nextSegment(&l_pSegment))
   {
      l_pSegRepPkt = new RsyncSegmentReportPacket(l_pSegment);
      l_pPacket    = new RsyncPacket(l_pSegRepPkt);
      
      if (!pushOutputPacket(l_pPacket))
      {
         printf("Failed to add seg #%d\n", l_nSegCount);
      }
      
      // The header packet is not longer needed, so delete it.
      delete l_pSegRepPkt;
      l_pSegRepPkt = NULL;
      
      l_nSegCount++;
      
      // Segment object is maintained by the RsyncSegmentReport,
      // so don't delete it.
      l_pSegment = NULL;
   }
   
   return true;
}
