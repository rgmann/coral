#ifndef SEGMENTER_UNIT_H
#define SEGMENTER_UNIT_H

#include <string>
#include "WorkUnit.h"
#include "RsyncSegmenter.h"

class SegmenterUnit : public WorkUnit
{
public:
   
   SegmenterUnit(Queue<RsyncPacket*>& outQueue);
   
   ~SegmenterUnit();
   
   void setRoot(const std::string& root);
   
   void setSegmentSize(ui32 nSegmentSizeBytes);
   
   void addFilename(const std::string& filename);
   
   RsyncSegmenter* getSegmenter();
   
   virtual bool work();
   
private:
   
   Queue<std::string>   m_filenameQueue;
   
   RsyncSegmenter       m_segmenter;
};

#endif // SEGMENTER_UNIT_H

// EOF