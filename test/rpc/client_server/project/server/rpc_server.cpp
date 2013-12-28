#include <unistd.h>
#include <signal.h>
#include <iostream>
#include "Thread.h"
#include "BinarySem.h"
#include "RpcClient.h"
#include "RpcServer.h"
#include "CalculatorServerStub.h"
#include "DatabaseServerStub.h"
#include "RpcServerListener.h"

using namespace liber::rpc;
using namespace liber::netapp;
using namespace rpc_getting_started;

bool  gbShutdown = false;

void sighandler(int s)
{
   gbShutdown = true;
}

int main()
{
  struct sigaction sigIntHandler;
  RpcServerListener server;

  sigIntHandler.sa_handler = sighandler;
  sigemptyset(&sigIntHandler.sa_mask);
  sigIntHandler.sa_flags = 0;
  sigaction(SIGINT, &sigIntHandler, NULL);

  // Set the port and start the server.
  server.setPort(14001);
  server.start();

  while (!gbShutdown)
  {
    sleep(1);
  }

  // Shutdown the server.
  server.stop();

  return 0;
}

