#include <string.h>
#include <iostream>

#include "TcpClient.h"
#include "NetAppPacket.h"
#include "TcpClientPacketRouter.h"

using namespace liber::net;
using namespace liber::netapp;
using namespace liber::concurrency;

//-----------------------------------------------------------------------------
TcpSocketReconnectTask::
TcpSocketReconnectTask(TcpSocket& rSocket, Mutex& rSocketLock)
: IThread("TcpSocketReconnectTask")
, mrSocket(rSocket)
, mrSocketLock(rSocketLock)
, mpHost(NULL)
, mnPort(-1)
{
}

//-----------------------------------------------------------------------------
void TcpSocketReconnectTask::setHost(const char* pHost, int nPort)
{
  mpHost = pHost;
  mnPort = nPort;
}

//-----------------------------------------------------------------------------
void TcpSocketReconnectTask::run(const bool& bShutdown)
{
  // Lock the socket while we attempt reconnection.
  mrSocketLock.lock();
  while (!bShutdown)
  {
    if (TcpClient::Connect(mrSocket, mpHost, mnPort, 1000)) break;
  }
  mrSocketLock.unlock();
}

//-----------------------------------------------------------------------------
// Class TcpClientPacketRouter definitions
//-----------------------------------------------------------------------------
TcpClientPacketRouter::TcpClientPacketRouter(bool bAutoReconnect)
: ClientPacketRouter()
, mnLockTimeoutMs(100)
, mbAutoReconnect(bAutoReconnect)
, mReconnectTask(mSocket, mSocketLock)
{
}

//----------------------------------------------------------------------------
TcpClientPacketRouter::~TcpClientPacketRouter()
{
  stop();
}

//-----------------------------------------------------------------------------
bool TcpClientPacketRouter::
start(const char* pHost, int nPort, ui32 txCapacity)
{
  bool lbSuccess = true;

  // Attept to connect to server
  if ((lbSuccess = TcpClient::Connect(mSocket, pHost, nPort, 1000)))
  {
    mReconnectTask.setHost(pHost, nPort);

    lbSuccess = ClientPacketRouter::start(txCapacity);
  }

  return lbSuccess;
}

//-----------------------------------------------------------------------------
TcpSocket& TcpClientPacketRouter::socket()
{
  return mSocket;
}

//-----------------------------------------------------------------------------
bool TcpClientPacketRouter::
setDisconnectCallback(liber::Callback* pCallback)
{
  bool lbSuccess = false;
  if ((lbSuccess = mCallbackLock.lock(0)))
  {
    mpDisconnectCallback = pCallback;
    mCallbackLock.unlock();
  }
  return lbSuccess;
}

//-----------------------------------------------------------------------------
bool TcpClientPacketRouter::writePacket(const NetAppPacket& rPacket)
{
  bool lbSuccess = false;

  if (mSocketLock.lock(mnLockTimeoutMs))
  {
    lbSuccess = mSocket.isConnected();
    mSocketLock.unlock();
  }

  if (lbSuccess)
  {
    SocketStatus lStatus;
    mSocket.write(lStatus, (char*)rPacket.basePtr(), rPacket.allocatedSize());

    if ((lbSuccess = (lStatus.status == SocketOk)))
    {
      lbSuccess = (lStatus.byteCount == rPacket.allocatedSize());
    }
    else
    {
      std::cout << "TcpClientPacketRouter::writePacket: Failure #"
                << lStatus.status << std::endl;
      processDisconnect();
    }
  }

  return lbSuccess;
}

//-----------------------------------------------------------------------------
bool TcpClientPacketRouter::readPacket(NetAppPacket& rPacket, int nTimeoutMs)
{
  bool lbSuccess = false;

  if (mSocketLock.lock(mnLockTimeoutMs))
  {
    lbSuccess = mSocket.isConnected();
    mSocketLock.unlock();
  }

  if (lbSuccess)
  {
    SocketStatus lStatus;
    NetAppPacket::Data lPacketHeader;

    mSocket.read(lStatus, (char*)&lPacketHeader, sizeof(lPacketHeader), nTimeoutMs);

    if ((lbSuccess = (lStatus.status == SocketOk)))
    {
      lbSuccess = (lStatus.byteCount == sizeof(lPacketHeader));

      if (lbSuccess && rPacket.allocate(lPacketHeader))
      {
        mSocket.read(lStatus, (char*)rPacket.dataPtr(),
                     lPacketHeader.length, nTimeoutMs);

        if ((lbSuccess = (lStatus.status == SocketOk)))
        {
          lbSuccess = (lStatus.byteCount == lPacketHeader.length);
        }
      }
    }

    if ((lStatus.status != SocketOk) && (lStatus.status != SocketTimeout))
    {
      std::cout << "TcpClientPacketRouter::readPacket: Failure #"
                << lStatus.status << std::endl;
      processDisconnect();
    }
  }

  return lbSuccess;
}

//-----------------------------------------------------------------------------
void TcpClientPacketRouter::processDisconnect()
{
  bool lbFirstToDisconnect = false;

  std::cout << "TcpClientPacketRouter::processDisconnect" << std::endl;

  if (mSocketLock.lock(mnLockTimeoutMs))
  {
    // Socket error occurred, so now perform an orderly disconnect.
    if (mSocket.isConnected())
    {
      lbFirstToDisconnect = true;
      mSocket.disconnect();
    }

    mSocketLock.unlock();
  }

  // If auto reconnect is enabled, launch the auto reconnect task on
  // a standalone thread.
  if (lbFirstToDisconnect)
  {
    if (mCallbackLock.lock(0))
    {
      if (mpDisconnectCallback)
      {
        mpDisconnectCallback->call(NULL);
      }

      mCallbackLock.unlock();
    }

    if (mbAutoReconnect)
    {
      mReconnectTask.launch();
    }
  }
}

