#include <unistd.h>
#include <signal.h>
#include "Log.h"
#include "ApplicationServer.h"
#include "EtermServerWorker.h"

using namespace liber::netapp;


class EtermServerWorkerCreator : public liber::netapp::WorkerCreator {
public:

  EtermServerWorkerCreator()
  : liber::netapp::WorkerCreator()
  , mController("10.0.0.18", 5013) {};

  ~EtermServerWorkerCreator() {};

  liber::netapp::ApplicationWorker* create ()
  {
    return new EtermServerWorker(mController);
  };

  eterm::HeimdallController mController;
};



bool  gbShutdown = false;

void sighandler(int s)
{
   gbShutdown = true;
}


int main()
{
  struct sigaction sigIntHandler;
  ApplicationServer server;

  liber::log::level(liber::log::Debug);

  sigIntHandler.sa_handler = sighandler;
  sigemptyset(&sigIntHandler.sa_mask);
  sigIntHandler.sa_flags = 0;
  sigaction(SIGINT, &sigIntHandler, NULL);

  // Set the port and start the server.
  server.start(14001, new EtermServerWorkerCreator());

  while (!gbShutdown)
  {
    sleep(1);
  }

  // Shutdown the server.
  server.stop();

  liber::log::flush();
  return 0;
}

