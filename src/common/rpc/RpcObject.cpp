#include <string>
#include <iostream>
#include <sstream>
#include "Log.h"
#include "PacketHelper.h"
#include "RpcObject.h"

using namespace liber;
using namespace liber::rpc;
using namespace liber::netapp;

//-----------------------------------------------------------------------------
RpcObject::RpcObject()
{
}

//-----------------------------------------------------------------------------
RpcObject::RpcObject(const std::string &resourceName, 
                     const std::string &actionName)
{
   mCallInfo.resource = resourceName;
   mCallInfo.action = actionName;
}

//-----------------------------------------------------------------------------
RpcObject::~RpcObject()
{
}

//-----------------------------------------------------------------------------
bool RpcObject::isValid() const
{
   return !mCallInfo.resource.empty() && !mCallInfo.action.empty();
}

//-----------------------------------------------------------------------------
RpcCallInfo& RpcObject::callInfo()
{
   return mCallInfo;
}

//-----------------------------------------------------------------------------
const RpcCallInfo& RpcObject::callInfo() const
{
   return mCallInfo;
}

//-----------------------------------------------------------------------------
RpcException& RpcObject::exception()
{
   mException.mCallInfo = callInfo();
   return mException;
}

//-----------------------------------------------------------------------------
void RpcObject::setParams(const PbMessage& message)
{
   message.SerializeToString(&mMessage);
}

//-----------------------------------------------------------------------------
void RpcObject::setParams(const std::string& message)
{
   mMessage.assign(message.data(), message.size());
}

//-----------------------------------------------------------------------------
void RpcObject::getParams(PbMessage& message) const
{
   message.ParseFromString(mMessage);
}

//-----------------------------------------------------------------------------
void RpcObject::getParams(std::string& message) const
{
   message.assign(mMessage.data(), mMessage.size());
}

//-----------------------------------------------------------------------------
bool RpcObject::getResponse(RpcObject &response) const
{
   if (!isValid()) return false;
   
   response.mCallInfo = mCallInfo;
   response.mException = mException;
   return true;
}

//-----------------------------------------------------------------------------
bool RpcObject::getResponse(RpcObject &response,
                            const PbMessage& message) const
{
   if (!getResponse(response)) return false;
   response.setParams(message);
   return true;
}

//-----------------------------------------------------------------------------
bool RpcObject::getResponse(RpcObject &response,
                            const std::string& message) const
{
   if (!getResponse(response)) return false;
   response.setParams(message);
   return true;
}

//-----------------------------------------------------------------------------
std::string RpcObject::serialize() const
{
   PacketCtor lPacket(NetworkByteOrder);

   lPacket.writeCString(callInfo().resource);
   lPacket.writeCString(callInfo().action);

   std::string lUiidData;
   lUiidData.assign((char*)callInfo().uiid.b, sizeof(Hash128));
   lPacket.write(lUiidData);
   lPacket.write((ui64)callInfo().rpcId);

   std::string lError = mException.serialize();
   lPacket.write(lError);

   lPacket.write(mMessage);

   return lPacket.stream.str();
}

//-----------------------------------------------------------------------------
bool RpcObject::deserialize(const std::string &data)
{
   PacketDtor lPacket(NetworkByteOrder);
   lPacket.setData(data);

   // Read the resource name
   if (lPacket.readCString(callInfo().resource) == PacketDtor::ReadFail)
   {
      log::error("RpcObject::deserialize failure at %d\n", __LINE__);
      return false;
   }

   // Read the action name.
   if (lPacket.readCString(callInfo().action) == PacketDtor::ReadFail)
   {
      log::error("RpcObject::deserialize failure at %d\n", __LINE__);
      return false;
   }

   std::string lUiidData;
   if (lPacket.read(lUiidData) != PacketDtor::ReadOk)
   {
      log::error("RpcObject::deserialize failure at %d\n", __LINE__);
      return false;
   }
   memcpy(callInfo().uiid.b, lUiidData.data(), lUiidData.size());

   if (!lPacket.read(callInfo().rpcId))
   {
      log::error("RpcObject::deserialize failure at %d\n", __LINE__);
      return false;
   }

   // Read the exception.
   std::string lException;
   if (lPacket.read(lException) == PacketDtor::ReadFail)
   {
      log::error("RpcObject::deserialize failure at %d\n", __LINE__);
      return false;
   }
   exception().deserialize(lException);

   // Read the protobuf message.
   if (lPacket.read(mMessage) == PacketDtor::ReadFail)
   {
      log::error("RpcObject::deserialize failure at %d\n", __LINE__);
      return false;
   }

   return true;
}

