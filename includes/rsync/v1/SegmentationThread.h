#ifndef SEGMENTATION_THREAD_H
#define SEGMENTATION_THREAD_H

#include "RsyncThread.h"
#include "RsyncSegmenter.h"

class SegmentationThread : public RsyncThread
{
public:
   
   SegmentationThread();
   
   ~SegmentationThread();
   
   void  setRoot(const std::string &root);
   
   bool  addFile(const std::string &filepath);

protected:
   
   void rsyncThreadFunc(ThreadArg* pArg);
   
private:
   
   RsyncSegmenter segmenter;
   
   Queue<std::string> filePathQueue;
};

#endif // SEGMENTATION_THREAD_H