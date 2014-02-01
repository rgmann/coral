#include "EtermServerListener.h"
#include "EtermServerWorker.h"

using namespace liber::netapp;
using namespace liber::net;

//-----------------------------------------------------------------------------
EtermServerListener::EtermServerListener()
: ServerListener()
{
}

//-----------------------------------------------------------------------------
EtermServerListener::~EtermServerListener()
{
}

//-----------------------------------------------------------------------------
ServerWorker* EtermServerListener::createWorker(liber::net::TcpSocket* pSocket)
{
  return new EtermServerWorker(pSocket);
}

