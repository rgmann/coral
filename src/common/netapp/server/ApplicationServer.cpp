#include "Log.h"
#include "ApplicationServer.h"

using namespace liber;
using namespace netapp;

//-----------------------------------------------------------------------------
ApplicationServer::ApplicationServer()
: mListener(mNodeManager)
{
}

//-----------------------------------------------------------------------------
ApplicationServer::~ApplicationServer()
{
}

//-----------------------------------------------------------------------------
bool ApplicationServer
::start(int nPort, WorkerCreator* pCreator, WorkerNodeComparator::Mode mode)
{
  bool lbSuccess = true;

  if (!mNodeManager.launch(mode))
  {
    log::error("ApplicationServer: Failed to launch WorkerNodeManager.\n");
    lbSuccess = false;
  }

  if (!mListener.launch(nPort, pCreator))
  {
    log::error("ApplicationServer: Failed to launch ConnectionListener.\n");
    lbSuccess = false;
  }

  return lbSuccess;
}

//-----------------------------------------------------------------------------
void ApplicationServer::stop()
{
  mListener.cancel( true );
  mNodeManager.cancel( true );
}



