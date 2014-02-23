#include "Log.h"
#include "TcpServer.h"
#include "ConnectionListener.h"

using namespace liber::net;
using namespace liber::netapp;

//-----------------------------------------------------------------------------
WorkerCreator::WorkerCreator() {}

//-----------------------------------------------------------------------------
WorkerCreator::~WorkerCreator() {}

//-----------------------------------------------------------------------------
ConnectionListener::ConnectionListener(WorkerNodeManager& rNodeManager)
: IThread("ConnectionListener")
, mpCreator(NULL)
, mrNodeManager(rNodeManager)
{
}

//-----------------------------------------------------------------------------
ConnectionListener::~ConnectionListener()
{
}

//-----------------------------------------------------------------------------
bool ConnectionListener::launch(int nPort, WorkerCreator* pCreator)
{
  bool lbSuccess = false;

  if (pCreator)
  {
    mpCreator = pCreator;
    mnPort = nPort;

    lbSuccess = launch();
  }

  return lbSuccess;
}

//-----------------------------------------------------------------------------
void ConnectionListener::run(const bool& bShutdown)
{
  TcpServer   lTcpServer;

  // Start the server on the specified port
  log::status("ConnectionListener: Starting server on port #%d\n", mnPort);

  // Start the TCP server.
  lTcpServer.start(mnPort);

  while (!bShutdown)
  {
    TcpSocket* lpSocket = lTcpServer.acceptClient(5000);

    if (lpSocket)
    {
      log::status("ConnectionListener: Creating worker for new connection.\n");

      // Use the supplied WorkerCreator to instantiate a new derived ApplicationWorker.
      ApplicationWorker* lpWorker = mpCreator->create();

      // If the worker was successfully created pass the worker to the
      // distribution and load balancing thread.
      if (lpWorker)
      {
        lpWorker->initialize(lpSocket);
        mrNodeManager.addWorker(lpWorker);
      }
    }
  }

  // Stop the TCP server
  lTcpServer.stop();

  log::status("ConnectionListener: Shutting down.\n");
}

