#include <fstream>
#include <iostream>
#include "Log.h"
#include "RsyncJob.h"
#include "SegmenterThread.h"
#include "AuthorityThread.h"
#include "AssemblerThread.h"

#define IMAGE

using namespace liber::rsync;

int main (int argc, char* argv[])
{
  //static const ui32 SegmentSize = 64;
  static const ui32 SegmentSize = 256;
  //static const ui32 SegmentSize = 6;

//  RsyncJob job;

  liber::log::level(liber::log::Debug);

  RsyncJob* job = new RsyncJob();
  FileSystemInterface fsif;

  fsif.setRoot("/Users/vaughanbiker/Development/liber/test/rsync/test_root");

#ifndef IMAGE
  job->descriptor().setDestination("client/file.dat");
  //job->descriptor().setDestination("client/instruction_test.cpp");
  job->descriptor().setSource("server/file.dat");
  //job->descriptor().setSource("server/instruction_test.cpp");
#else
  job->descriptor().setDestination("client/image.png");
  job->descriptor().setSource("server/image.png");
#endif

  job->descriptor().setSegmentSize(SegmentSize);

  SegmenterThread segmenter(fsif);
  AuthorityThread authority(fsif);
  AssemblerThread assembler(fsif);

  segmenter.launch();
  authority.launch();
  assembler.launch();

  segmenter.addJob(job);
  authority.addJob(job);
  assembler.addJob(job);

  liber::log::status("Waiting for job to complete.\n");
  job->waitDone();

  segmenter.cancel(true);
  authority.cancel(true);
  assembler.cancel(true);

  job->report().print(std::cout);
  delete job;

  liber::log::flush();
  return 0;
}

