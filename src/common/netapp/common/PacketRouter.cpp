#include "PacketRouter.h"

using namespace liber::netapp;

//-----------------------------------------------------------------------------
PacketRouter::PacketRouter(Queue<NetAppPacket*>* pOutQueue)
: mpOutQueue(pOutQueue)
{
}

//-----------------------------------------------------------------------------
PacketRouter::~PacketRouter()
{
  mpOutQueue = NULL;
}

//-----------------------------------------------------------------------------
bool PacketRouter::
addSubscriber(int subscriberId, PacketSubscriber* pSubscriber)
{
  bool lbSuccess = true;

  mTableLock.lock();
  lbSuccess = (mSubscriberTable.count(subscriberId) == 0);
  mTableLock.unlock();
  if (!lbSuccess)
  {
    printf("PacketRouter::addSubscriber: "
           "Router already has a subcriber with ID = %d\n", subscriberId);
  }

  lbSuccess &= (pSubscriber != NULL);
  if (lbSuccess)
  {
    lbSuccess = mTableLock.lock();
    if (lbSuccess)
    {
      pSubscriber->setId(subscriberId);
      pSubscriber->setOutputQueue(mpOutQueue);
      mSubscriberTable.insert(std::make_pair(subscriberId, pSubscriber));
      lbSuccess = true;
      mTableLock.unlock();
    }
  }

  return lbSuccess;
}

//-----------------------------------------------------------------------------
PacketSubscriber* PacketRouter::removeSubscriber(int subscriberId)
{
  PacketSubscriber* lpSubscriber = NULL;
  bool lbSuccess = (mSubscriberTable.count(subscriberId) != 0);

  lbSuccess &= (lpSubscriber != NULL);
  if (lbSuccess)
  {
    lbSuccess = mTableLock.lock();
    if (lbSuccess)
    {
      lpSubscriber = mSubscriberTable.find(subscriberId)->second;
      mSubscriberTable.erase(subscriberId);
      lbSuccess = true;
      mTableLock.unlock();
    }
  }

  return lpSubscriber;
}

//-----------------------------------------------------------------------------
ui32 PacketRouter::count()
{
  return mSubscriberTable.size();
}

//-----------------------------------------------------------------------------
bool PacketRouter::empty()
{
  return mSubscriberTable.empty();
}

//-----------------------------------------------------------------------------
bool PacketRouter::hasSubscriber(int subscriberId)
{
  mTableLock.lock();
  bool lbExists = (mSubscriberTable.count(subscriberId) != 0);
  mTableLock.unlock();
  return lbExists;
}

//-----------------------------------------------------------------------------
PacketSubscriber* PacketRouter::getSubscriber(int subscriberId)
{
  PacketSubscriber* lpSubscriber = NULL;

  mTableLock.lock();
  if (mSubscriberTable.count(subscriberId) != 0)
  {
    lpSubscriber = mSubscriberTable.find(subscriberId)->second;
  }
  mTableLock.unlock();

  return lpSubscriber;
}


