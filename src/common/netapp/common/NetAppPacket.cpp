#include <stdio.h>
#include "NetAppPacket.h"

using namespace liber::netapp;

//-----------------------------------------------------------------------------
NetAppPacket::NetAppPacket()
{
}

//-----------------------------------------------------------------------------
NetAppPacket::NetAppPacket(int type, ui32 length)
: GenericPacket(sizeof(NetAppPacket::Data), length)
{
  if (isAllocated())
  {
    data()->type = type;
    data()->length = length;
  }
}

//-----------------------------------------------------------------------------
bool NetAppPacket::allocate(const NetAppPacket::Data& rData)
{
  bool lbSuccess = GenericPacket::allocate(sizeof(NetAppPacket::Data), 
                                           rData.length);

  if (lbSuccess)
  {
    data()->type = rData.type;
    data()->length = rData.length;
  }

  return lbSuccess;
}

//-----------------------------------------------------------------------------
NetAppPacket::Data* const NetAppPacket::data()
{
  Data* lpHeader = NULL;

  if (isAllocated())
  {
    lpHeader = reinterpret_cast<Data*>(basePtr());
  }

  return lpHeader;
}

//-----------------------------------------------------------------------------
/*void* NetAppPacket::dataPtr()
{
  char* lpData = NULL;
  if (isAllocated())
  {
    lpData = reinterpret_cast<char*>(basePtr());
    lpData += sizeof(Data);
  }
  return lpData;
}*/

//-----------------------------------------------------------------------------
/*void* const NetAppPacket::dataPtr() const
{
  char* lpData = NULL;
  if (isAllocated())
  {
    lpData = reinterpret_cast<char*>(basePtr());
    lpData += sizeof(Data);
  }
  return (void* const)lpData;
}*/

