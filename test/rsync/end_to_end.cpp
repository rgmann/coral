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
    std::cout << "\n\n";

    mSem.give();
  }

  CountingSem mSem;
  const char* mpName;
};

#define LOCAL_TEST
#define FILE_TEST
#define REMOTE_SRC_TEST
#define DFILE_TEST

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

#ifdef LOCAL_TEST
    jobStatus = local.sync("d_inst.cpp", "s_inst.cpp");
    if (jobStatus == RsyncSuccess)
    {
      localCallback.mSem.take();
    }
    else
    {
      log::error("RSYNC job failed: %s\n", liber::rsync::errorToString(jobStatus).c_str()); 
    }
#endif

#ifdef FILE_TEST
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
#endif

#ifdef REMOTE_SRC_TEST
    jobStatus = local.sync("instruction_test.cpp", "instruction_test.cpp", false, true);
    if (jobStatus == RsyncSuccess)
    {
      localCallback.mSem.take();
    }
    else
    {
      log::error("RSYNC job failed: %s\n", liber::rsync::errorToString(jobStatus).c_str()); 
    }
#endif

#ifdef DFILE_TEST
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
#endif
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

