#include <fstream>
#include <iostream>
#include "Log.h"
#include "RsyncNode.h"
#include "IntraRouter.h"

#define IMAGE
#define RSYNC_SUB_ID   1

using namespace liber;
using namespace liber::rsync;
using namespace liber::netapp;

class TestCallback : public RsyncJobCallback {
public:

  TestCallback(const char* pName) : mSem(0), mpName(pName) {};

  void call(const JobDescriptor& job, const JobReport& report)
  {
    log::status("%s: Completed %s\n", mpName, job.getDestination().path.string().c_str());

    liber::log::flush();
    report.print(std::cout);

    mSem.give();
  }

  CountingSem mSem;
  const char* mpName;
};

int main (int argc, char* argv[])
{
  liber::log::level(liber::log::Verbose);
  liber::log::options(0);

  IntraRouter localRouter;
  IntraRouter remoteRouter;

  localRouter.setCounterpart(&remoteRouter);
  remoteRouter.setCounterpart(&localRouter);

  localRouter.launch();
  remoteRouter.launch();

  TestCallback localCallback("Local");
  TestCallback remoteCallback("Remote");
  RsyncNode local("/Users/vaughanbiker/Development/liber/test/rsync/test_root/client");
  RsyncNode remote("/Users/vaughanbiker/Development/liber/test/rsync/test_root/server");

  if (localRouter.addSubscriber(RSYNC_SUB_ID, &local.subscriber()) &&
      remoteRouter.addSubscriber(RSYNC_SUB_ID, &remote.subscriber()))
  {
    RsyncError jobStatus = RsyncSuccess;

    local.setCallback(&localCallback);
    remote.setCallback(&remoteCallback);
    //local.sync("image.png", "image.png", true);
    jobStatus = local.sync("file.dat", "file.dat", true, false);
    if (jobStatus == RsyncSuccess)
    {
      localCallback.mSem.take();
      remoteCallback.mSem.take();
    }
    else
    {
      log::error("RSYNC job failed: %s\n", liber::rsync::errorToString(jobStatus).c_str()); 
    }

    //jobStatus = local.sync("instruction_test.cpp", "instruction_test.cpp", false, true);
    jobStatus = local.sync("instruction_test.cpp", "instruction_test.cpp", true, false);
    if (jobStatus == RsyncSuccess)
    {
      localCallback.mSem.take();
      remoteCallback.mSem.take();
    }
    else
    {
      log::error("RSYNC job failed: %s\n", liber::rsync::errorToString(jobStatus).c_str()); 
    }

    jobStatus = local.sync("dfile_0.dat", "dfile_0.dat", false, true);
    if (jobStatus == RsyncSuccess)
    {
      localCallback.mSem.take();
      //remoteCallback.mSem.take();
    }
    else
    {
      log::error("RSYNC job failed: %s\n", liber::rsync::errorToString(jobStatus).c_str()); 
    }
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

