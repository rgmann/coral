#include <fstream>
#include <iostream>
#include "Log.h"
#include "RsyncNode.h"
#include "IntraRouter.h"

#define IMAGE
#define NODE_LOCAL_ID   1
#define NODE_SERVICE_ID 2

using namespace liber;
using namespace liber::rsync;
using namespace liber::netapp;

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
  liber::log::options(0);

  IntraRouter localRouter;
  IntraRouter remoteRouter;

  localRouter.setCounterpart(&remoteRouter);
  remoteRouter.setCounterpart(&localRouter);

  localRouter.launch();
  remoteRouter.launch();

  TestCallback callback;
  RsyncNode local("/Users/vaughanbiker/Development/liber/test/rsync/test_root/client");
  RsyncNode remote("/Users/vaughanbiker/Development/liber/test/rsync/test_root/server");

  if (local.registerSubscribers(localRouter, NODE_LOCAL_ID, NODE_SERVICE_ID) &&
      remote.registerSubscribers(remoteRouter, NODE_LOCAL_ID, NODE_SERVICE_ID))
  {
    local.setCompletionCallback(&callback);
    //local.sync("image.png", "image.png", true);
    //local.sync("client/file.dat", "server/file.dat");
    local.sync("instruction_test.cpp", "instruction_test.cpp", true);

    callback.mSem.take();
    //callback.mSem.take();
    //callback.mSem.take();
  }
  else
  {
    log::error("Failed to register node.\n");
  }

  localRouter.cancel(true);
  remoteRouter.cancel(true);

  liber::log::flush();
  return 0;
}

