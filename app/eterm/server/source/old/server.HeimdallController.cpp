#include <iostream>
#include "HeimdallController.h"
#include "NetAppPacket.h"
#include "HeimdallLedStateCommand.h"

using namespace eterm;
using namespace liber::net;
using namespace liber::netapp;

//-----------------------------------------------------------------------------
HeimdallController::HeimdallController()
: mpSocket(NULL)
, mpServerThread(NULL)
, mnPort(5013)
{
  mpServerThread = Thread::Create(ServerListenThreadEntry, this);
}

//-----------------------------------------------------------------------------
HeimdallController::~HeimdallController()
{
  if (mpServerThread)
  {
    mpServerThread->stop();
    mpServerThread->join();
    delete mpServerThread;
    mpServerThread = NULL;
  }

  mSocketLock.lock();
  destroySocket();
  mSocketLock.unlock();
}

//------------------------------------------------------------------------------
void HeimdallController::setPort(int nPort)
{
  mnPort = nPort;
}

//------------------------------------------------------------------------------
bool HeimdallController::isConnected() const
{
  return (mpSocket != NULL);
}

//------------------------------------------------------------------------------
bool HeimdallController::
setLedState(LedId id, bool bOn, HeimdallStatus::Data& status)
{
  bool lbSuccess = false;
  eterm::HeimdallLedStateCommand lCommand;

  lCommand.data()->ledId = id;
  lCommand.data()->ledOn = bOn;

  if (sendCommand(HdallSetLedState, lCommand.basePtr(), lCommand.allocatedSize()))
  {
    lbSuccess = recvResponse(&status, sizeof(status));
  }

  return lbSuccess;
}

//------------------------------------------------------------------------------
bool HeimdallController::
getFingerprintStatus(HeimdallStatus::Data& hstatus, eterm::FingerprintStatus::Data& fstatus)
{
  bool lbSuccess = false;

  if (sendCommand(HdallFingerprintStatus, NULL, 0))
  {
    lbSuccess = recvResponse(&hstatus, sizeof(hstatus));
    if (lbSuccess && hstatus.success)
    {
      lbSuccess &= recvResponse(&fstatus, sizeof(fstatus));
    }
  }

  return lbSuccess;
}

//------------------------------------------------------------------------------
bool HeimdallController::
sendCommand(i32 commandId, const void* pData, ui32 nBytes)
{
  bool lbSuccess = false;
  SocketStatus lStatus;

  if (mSocketLock.lock(50))
  {
    if (mpSocket)
    {
      NetAppPacket packet(commandId, nBytes);

      if (pData && (nBytes > 0)) memcpy(packet.dataPtr(), pData, nBytes);
      //packet.swapByteOrder();
      //printf("HeimdallController: type = 0x%08X\n", packet.data()->type);
      //printf("HeimdallController: length = 0x%08X\n", packet.data()->length);
      mpSocket->write(lStatus, (char*)packet.basePtr(), packet.allocatedSize());

      if (lStatus.status == SocketOk)
      {
        std::cout << "HeimdallController::sendCommand: success" << std::endl;
        lbSuccess = true;
      }
      else
      {
        destroySocket();
      }
    }

    mSocketLock.unlock();
  }

  return lbSuccess;
}

//------------------------------------------------------------------------------
bool HeimdallController::recvResponse(void* pData, ui32 nBytes)
{
  bool lbSuccess = false;
  SocketStatus lStatus;
  NetAppPacket::Data lHeader;

  if (mSocketLock.lock(50))
  {
    if (mpSocket)
    {
      // Attempt to read the packet header.
      mpSocket->read(lStatus, (char*)&lHeader, sizeof(lHeader), 1000);
      std::cout << "HeimdallController::recvRsponse: status = " << (int)lStatus.status << std::endl;
      if (lStatus.status == SocketOk)
      {
        lbSuccess = (lStatus.byteCount == sizeof(lHeader));

        std::cout << "HeimdallController::recvRsponse: byteCount = " << (int)lStatus.byteCount << std::endl;
        if (lbSuccess)
        {
          std::cout << "HeimdallController::recvResponse: type = "
                    << lHeader.type << ", length = " << lHeader.length << std::endl;
          if (lHeader.length == nBytes)
          {
            mpSocket->read(lStatus, (char*)pData, lHeader.length, 1000);

            if (lStatus.status == SocketOk)
            {
              lbSuccess = true;
            }
            else
            {
              destroySocket();
            }
          }
        }
      }
      else
      {
        destroySocket();
      }
    }

    mSocketLock.unlock();
  }

  return lbSuccess;
}

//------------------------------------------------------------------------------
void HeimdallController::destroySocket()
{
  if (mpSocket)
  {
    std::cout << "HeimdallController::destroySocket" << std::endl;
    mpSocket->disconnect();
    delete mpSocket;
    mpSocket = NULL;
  }
}

//------------------------------------------------------------------------------
void HeimdallController::ServerListenThreadEntry(ThreadArg* pArg)
{
  ((HeimdallController*)pArg->pUserData)->serverListenThread(pArg);
}

//-----------------------------------------------------------------------------
void HeimdallController::serverListenThread(ThreadArg* pArg)
{
  TcpServer lTcpServer;

  lTcpServer.start(mnPort);

  while (!pArg->stopSignalled())
  {
    if (mSocketLock.lock(500))
    {
      if (mpSocket == NULL)
      {
        mpSocket = lTcpServer.acceptClient(50);
        if (mpSocket)
        {
          std::cout << "Connected to Heimdall!" << std::endl;
        }
      }

      mSocketLock.unlock();
    }
  }

  lTcpServer.stop();
}


