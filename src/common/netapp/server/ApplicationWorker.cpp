#include "Log.h"
#include "PacketReceiver.h"
#include "ApplicationWorker.h"

using namespace liber::net;
using namespace liber::netapp;

//------------------------------------------------------------------------------
ApplicationWorker::ApplicationWorker()
: mpSocket(NULL)
, mnLockTimeoutMs(100)
, mRouter(&mInQueue, &mOutQueue)
, mEnqueueTimeoutMs(DefaultEnqueueTimeoutMs)
//, mDequeueTimeoutMs(DefaultDequeueTimeoutMs)
{
}

//------------------------------------------------------------------------------
ApplicationWorker::~ApplicationWorker()
{
}

//------------------------------------------------------------------------------
bool ApplicationWorker::initialize(TcpSocket* pSocket)
{
  bool lbSuccess = true;

  // Store the socket pointer. 
  mpSocket = pSocket;

  // Register the keepalive subscriber.
  lbSuccess &= mRouter.subscribe(ConnectionStatus::SubscriberId,
                                     &mConnectionStatus);

  //
  lbSuccess &= derivedInitialize();

  return lbSuccess;
}

//-----------------------------------------------------------------------------
bool ApplicationWorker::derivedInitialize()
{
  printf("ApplicationWorker::derivedInitialize() is unimplemented\n");
  return true;
}

//-----------------------------------------------------------------------------
void ApplicationWorker::destroy()
{
  if (mpSocket)
  {
    printf("ApplicationWorker closing socket\n");
    mpSocket->disconnect();

    delete mpSocket;
    mpSocket = NULL;
  }

  NetAppPacket* lpPacket = NULL;
  while (mInQueue.pop(lpPacket, 5))
  {
    if (lpPacket) delete lpPacket;
    lpPacket = NULL;
  }

  PacketContainer* lpContainer = NULL;
  while ((lpContainer = mOutQueue.pop(5)) != NULL)
  {
    if (lpContainer) delete lpContainer;
    lpContainer = NULL;
  }

  // Unregister the keepalive subscriber.
  mRouter.unsubscribe(ConnectionStatus::SubscriberId, &mConnectionStatus );

  derivedDestroy();
}

//------------------------------------------------------------------------------
const ConnectionStatus& ApplicationWorker::status() const
{
  return mConnectionStatus;
}

//------------------------------------------------------------------------------
void ApplicationWorker::derivedDestroy()
{
  printf("ApplicationWorker::derivedDestroy() is unimplemented\n");
}

//------------------------------------------------------------------------------
bool ApplicationWorker::readPacket(int nTimeoutMs, bool bRouteOnSuccess)
{
  bool lbEndOfStream = false;
  SocketStatus lStatus;

  NetAppPacket::Data lPacketHeader;

  if (mSocketLock.lock(mnLockTimeoutMs))
  {
    lbEndOfStream = (mpSocket->isConnected() == false);
    mSocketLock.unlock();
  }
  else
  {
    // The status of the socket is indeterminate, so don't read a packet,
    // but also don't automatically assume that the socket is disconnected.
    // Try again later.
    //return true;
  }

  if (!lbEndOfStream)
  {
    // Each worker has a standard application specific header.  Each
    // packet should be preceded by a header and the header should
    // indicate the length of the following data.
    mpSocket->read(lStatus, (char*)&lPacketHeader, sizeof(lPacketHeader), nTimeoutMs);

    if (lStatus.status == SocketOk)
    {
      if ((lStatus.byteCount == sizeof(lPacketHeader)) && lPacketHeader.length > 0)
      {
        bool lbSuccess = false;
        // Pass the header to the worker so that it can parse the
        // expected payload length.
        NetAppPacket* lpPacket = new NetAppPacket();
        if (lpPacket->allocate(lPacketHeader))
        {
          mpSocket->read(lStatus, (char*)lpPacket->dataPtr(),
                         lPacketHeader.length, nTimeoutMs);

          lbSuccess =  (lStatus.status == SocketOk);
          lbSuccess &= (lStatus.byteCount == lPacketHeader.length);

          if (lbSuccess)
          {
            mConnectionStatus.sampleRecvTime();

            log::debug("ApplicationWorker::readPacket: type = %d, len = %d\n",
                       lPacketHeader.type,
                       lPacketHeader.length);

            // The ApplicationWorker can route the packet immediately or defer.
            if (bRouteOnSuccess)
            {
              mRouter.route(lpPacket);
            }
            else
            {
              lbSuccess = mInQueue.push(lpPacket, mEnqueueTimeoutMs);
              if (!lbSuccess)
              {
                log::warn("ApplicationWorker::readPacket: "
                          "Failed to enqueue new packet to worker.\n");
              }
            }
          }
          else
          {
            log::warn("ApplicationWorker::readPacket: "
                      "Timed out receiving packet body "
                      "(expected = %d, received = %d)\n",
                      lPacketHeader.length, lStatus.byteCount);
          }
        }
        else
        {
          log::warn("ApplicationWorker::readPacket: "
                    "Failed to allocate NetAppPacket!\n");
        }

        // If the packet was allocated, but we failed to read or route the
        // packet, ownership of the packet is still local, so delete it.
        if (lpPacket && !lbSuccess)
        {
          delete lpPacket;
        }
      }
    }

    // If socket error occurred (excluding a read timeout), close the socket
    // and flag the end of the input packet stream.
    if ((lStatus.status != SocketOk) && (lStatus.status != SocketTimeout))
    {
      log::debug("ApplicationWorker::readPacket: end of stream\n");
      disconnect();
      lbEndOfStream = true;
    }
  }

 

  return (lbEndOfStream == false);
}

//-----------------------------------------------------------------------------
bool ApplicationWorker::writePacket(int nTimeoutMs)
{
  bool lbEndOfStream = false;

  if (mSocketLock.lock(mnLockTimeoutMs))
  {
    lbEndOfStream = (mpSocket->isConnected() == false);
    mSocketLock.unlock();
  }
  else
  {
    // The status of the socket is indeterminate, so don't read a packet,
    // but also don't automatically assume that the socket is disconnected.
    // Try again later.
    log::debug("ApplicationWorker::writePacket: socket locked\n");
  }

  if (!lbEndOfStream)
  {
    PacketContainer* lpContainer = NULL;
    if ((lpContainer = mOutQueue.pop(nTimeoutMs)) != NULL)
    {
      NetAppPacket packet(lpContainer->destination_id_,
                          lpContainer->packet_ptr_->allocatedSize());

      memcpy(packet.dataPtr(),
             lpContainer->packet_ptr_->basePtr(),
             lpContainer->packet_ptr_->allocatedSize());

      SocketStatus lStatus;

      // Send the message
      mpSocket->write(lStatus, (char*)packet.basePtr(), packet.allocatedSize());
      delete lpContainer;

      if (lStatus.status != SocketOk)
      {
        log::error("ApplicationWorker::writePacket: "
                   "Send failed with error #%d\n", lStatus.status);
        disconnect();
        lbEndOfStream = true;
      }
    }
  }
  else
  {
    log::debug("ApplicationWorker::writePacket: end of stream\n");
  }

  return (lbEndOfStream == false);
}

//-----------------------------------------------------------------------------
void ApplicationWorker::routePacket()
{
  NetAppPacket* lpPacket = NULL;
  if (mInQueue.pop(lpPacket, 0) && lpPacket)
  {
    mRouter.route(lpPacket);
  }
}

//-----------------------------------------------------------------------------
void ApplicationWorker::disconnect()
{
  if (mSocketLock.lock())
  {
    // Socket error occurred, so now perform an orderly disconnect.
    if (mpSocket->isConnected())
    {
      mpSocket->disconnect();
    }

    log::debug("ApplicationWorker: disconnecting\n");
    mSocketLock.unlock();
  }
}

//-----------------------------------------------------------------------------
TcpSocket* const ApplicationWorker::socket()
{
  return mpSocket;
}

//-----------------------------------------------------------------------------
ServerPacketRouter& ApplicationWorker::router()
{
  return mRouter;
}

