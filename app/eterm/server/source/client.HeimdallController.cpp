#include <string.h>
#include <iostream>
#include "client.HeimdallController.h"
#include "NetAppPacket.h"
#include "TcpClient.h"
#include "HeimdallLedStateCommand.h"

#define HOST_NAME "10.0.0.18"

using namespace eterm;
using namespace liber::net;
using namespace liber::netapp;

//-----------------------------------------------------------------------------
HeimdallController::HeimdallController()
: mpSocket(NULL)
, mnPort(5013)
{
}

//-----------------------------------------------------------------------------
HeimdallController::~HeimdallController()
{
  mSocketLock.lock();
  disconnect();
  mSocketLock.unlock();
}

//------------------------------------------------------------------------------
void HeimdallController::setPort(int nPort)
{
  mnPort = nPort;
}

//------------------------------------------------------------------------------
HeimdallController::Status HeimdallController::
setLedState(LedId id, bool bOn, HeimdallStatus::Data& status)
{
  Status lStatus;
  lStatus.success = false;
  eterm::HeimdallLedStateCommand lCommand;

  lCommand.data()->ledId = id;
  lCommand.data()->ledOn = bOn;

  if ((lStatus.connected = connect()))
  {
    if (sendCommand(HdallSetLedState, lCommand.basePtr(), lCommand.allocatedSize()))
    {
      lStatus.success = recvResponse(&status, sizeof(status));
    }

    disconnect();
  }

  return lStatus;
}

//------------------------------------------------------------------------------
HeimdallController::Status HeimdallController::
getFingerprintStatus(HeimdallStatus::Data& hstatus, eterm::FingerprintStatus::Data& fstatus)
{
  Status lStatus;
  lStatus.success = false;

  if ((lStatus.connected = connect()))
  {
    if (sendCommand(HdallFingerprintStatus, NULL, 0))
    {
      lStatus.success = recvResponse(&hstatus, sizeof(hstatus));
      if (lStatus.success && hstatus.success)
      {
        lStatus.success = recvResponse(&fstatus, sizeof(fstatus));
      }
    }

    disconnect();
  }

  return lStatus;
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
      /*else
      {
        destroySocket();
      }*/
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
            /*else
            {
              destroySocket();
            }*/
          }
        }
      }
      /*else
      {
        destroySocket();
      }*/
    }

    mSocketLock.unlock();
  }

  return lbSuccess;
}

//------------------------------------------------------------------------------
bool HeimdallController::connect()
{
  bool lbSuccess = false;

  if (mSocketLock.lock(50))
  {
    mpSocket = TcpClient::Connect(HOST_NAME, mnPort, 1000);
    lbSuccess = (mpSocket != NULL);

    if (lbSuccess) std::cout << "HeimdallController::connect(): SUCCESS" << std::endl;
    else std::cout << "HeimdallController::connect(): FAILURE" << std::endl;

    mSocketLock.unlock();
  }

  return lbSuccess;
}

//------------------------------------------------------------------------------
void HeimdallController::disconnect()
{
  if (mSocketLock.lock(50) && mpSocket)
  {
    std::cout << "HeimdallController::disconnect()" << std::endl;
    mpSocket->disconnect();
    delete mpSocket;
    mpSocket = NULL;

    mSocketLock.unlock();
  }
}


