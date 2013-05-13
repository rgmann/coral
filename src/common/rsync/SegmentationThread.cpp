#include "SegmentationThread.h"

//------------------------------------------------------------------------------
SegmentationThread::SegmentationThread()
{
   
}

//------------------------------------------------------------------------------
SegmentationThread::~SegmentationThread()
{
   
}

//------------------------------------------------------------------------------
void SegmentationThread::setRoot(const std::string &root)
{
   segmenter.setRoot(root);
}

//------------------------------------------------------------------------------
bool SegmentationThread::addFile(const std::string &filepath)
{
   return filePathQueue.push(filepath, 100);
}

//------------------------------------------------------------------------------
void SegmentationThread::rsyncThreadFunc(ThreadArg* pArg)
{
   std::string          filepath;
   RsyncSegmentReport*  pReport = NULL;

   while (!pArg->stopSignalled())
   {
      if (!filePathQueue.pop(filepath, 100))
      
      if (!segmenter.process(filepath))
      {
         continue;
      }
      
      pReport = segmenter.getReport(filepath);
      if (pReport == NULL)
      {
         continue;
      }
      
//      if (!pReport->header(&pHeader))
//      {
//         continue;
//      }
//      pushPacket(pHeader);
//      
//      while (pReport->nextSegment(&pSegment))
//      {
//         pushPacket(pSegment);
//      }
   }
}
