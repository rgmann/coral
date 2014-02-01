#include "RsyncSegmenter.h"
#include "RsyncHelper.h"
#include <iostream>
#include <stdio.h>

//------------------------------------------------------------------------------
RsyncSegmenter::RsyncSegmenter()
{
   m_sRootDir = "";
}

//------------------------------------------------------------------------------
RsyncSegmenter::~RsyncSegmenter()
{
}

//------------------------------------------------------------------------------
void RsyncSegmenter::setRoot(const std::string &root)
{
   m_sRootDir = root;
}

//------------------------------------------------------------------------------
void RsyncSegmenter::setSegmentSize(unsigned int size)
{
   m_nBlockSize = size;
}

//------------------------------------------------------------------------------
bool RsyncSegmenter::process(const std::string &filename)
{
   // Segment the file.
   RsyncSegmentReport*  l_pReport = NULL;
   std::string          l_sFullFilePath;
   
   l_pReport = RsyncSegmentReport::Build(m_sRootDir, filename, m_nBlockSize);
   
   if (l_pReport == NULL)
   {
      return false;
   }
   
   // Add the segment list to the file lookup map.  If the file is
   // already in the map, then we need to replace it?
   if (m_mSegReportMap.count(filename) != 0)
   {
      return false;
   }
   
   // Add the report to the map
   m_mSegReportMap[filename] = l_pReport;
   
   return true;
}

//------------------------------------------------------------------------------
RsyncSegmentReport* RsyncSegmenter::getReport(const std::string &filename)
{
   if (m_mSegReportMap.count(filename) > 0)
   {
      return m_mSegReportMap[filename];
   }
   
   return NULL;
}



