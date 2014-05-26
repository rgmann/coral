#include <fstream>
#include <iostream>
#include "Log.h"
#include "RsyncJob.h"
#include "SegmenterThread.h"
#include "AuthorityThread.h"
#include "AssemblerThread.h"
#include "SegmentHook.h"

#define IMAGE

using namespace liber::rsync;

class TestHook : public liber::rsync::SegmentHook {
public:

  void call(Segment& rSegment) {};

};

int main (int argc, char* argv[])
{
  //static const ui32 SegmentSize = 6;
  //static const ui32 SegmentSize = 64;
  static const ui32 SegmentSize = 256;

//  RsyncJob job;

  liber::log::level(liber::log::Debug);

  RsyncJob* job = new RsyncJob();
  FileSystemInterface fsif;

  fsif.setRoot("/Users/vaughanbiker/Development/liber/test/rsync/test_root");

  SegmentQueue segments;

#ifndef IMAGE
  job->descriptor().setDestination("client/file.dat");
  //job->descriptor().setDestination("client/instruction_test.cpp");
  job->descriptor().setSource("server/file.dat");
  //job->descriptor().setSource("server/instruction_test.cpp");
#else
  job->descriptor().setDestination("client/image.png");
  job->descriptor().setSource("server/image.png");
#endif

  SegmentationReport report;
  Segmenter destSegmenter(Segmenter::FullStride);
  std::ifstream stream;

  if (fsif.open(job->descriptor().getDestination(), stream))
  {
    destSegmenter.process(stream,
                      segments,
                      SegmentSize,
                      report);
    stream.close();
  }

  Segment* lpSegment = NULL;
  while (segments.pop(&lpSegment, 100) && lpSegment)
  {
    delete lpSegment;
    lpSegment = NULL;
  }

  report.print(std::cout, 0);


  std::ofstream log("segment_times.csv");

  TestHook testHook;
  Segmenter sourceSegmenter(Segmenter::EveryOffset);
  for (ui32 segSize = 8; segSize <= 8192; segSize *= 2)
  {
    if (fsif.open(job->descriptor().getSource(), stream))
    {
      sourceSegmenter.process(stream,
                        testHook,
                        segSize,
                        report);
      stream.close();
    }
    log << segSize << "," << (report.end - report.begin).fseconds() << "\n";
  }
  log.close();

  lpSegment = NULL;
  /*while (segments.pop(&lpSegment, 100) && lpSegment)
  {
    delete lpSegment;
    lpSegment = NULL;
  }*/
  report.print(std::cout, 0);

  delete job;
  return 0;
}
