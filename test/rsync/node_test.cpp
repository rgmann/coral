#include <fstream>
#include <iostream>
#include "Log.h"
#include "RsyncNode.h"

#define IMAGE

using namespace liber;
using namespace liber::rsync;

class TestCallback : public RsyncJobCallback {
public:

  TestCallback() : mSem(0) {};

  void call(const JobDescriptor& job, const JobReport& report)
  {
    log::status("Completed %s\n", job.getDestination().string().c_str());

    liber::log::flush();
    report.print(std::cout);

    mSem.give();
  }

  CountingSem mSem;
};

int main (int argc, char* argv[])
{
  liber::log::level(liber::log::Debug);

  TestCallback callback;
  RsyncNode node("/Users/vaughanbiker/Development/liber/test/rsync/test_root");

  node.setCompletionCallback(&callback);
  node.sync("client/image.png", "server/image.png");
  node.sync("client/file.dat", "server/file.dat");
  node.sync("client/instruction_test.cpp", "server/instruction_test.cpp");

  callback.mSem.take();
  callback.mSem.take();
  callback.mSem.take();

  liber::log::flush();
  return 0;
}

