#ifndef RSYNC_SEGMENTER_H
#define RSYNC_SEGMENTER_H

#include <map>
#include <string>
#include <fstream>
#include "RsyncSegmentReport.h"


class RsyncSegmenter
{
public:
   
   RsyncSegmenter();
   
   ~RsyncSegmenter();
   
   void  setRoot(const std::string &root);
   
   void  setSegmentSize(unsigned int size);
   
   bool  process(const std::string &filename);
   
   RsyncSegmentReport*  getReport(const std::string &filename);
   
private:
   
   RsyncSegmenter(const RsyncSegmenter &other);
   
   RsyncSegmenter & operator=(const RsyncSegmenter &rhs);
      
private:
   
   unsigned int m_nBlockSize;
   
   std::string    m_sRootDir;
   
   std::map<std::string, RsyncSegmentReport*> m_mSegReportMap;
};


#endif // RSYNC_SEGMENTER_H