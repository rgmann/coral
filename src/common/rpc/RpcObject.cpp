#include <string>
#include <iostream>
#include <sstream>
#include "Log.h"
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
void RpcObject::pack(liber::netapp::SerialStream& stream) const
{
   stream.writeCString(callInfo().resource);
   stream.writeCString(callInfo().action);

   stream.write((const char*)callInfo().uuid.data, callInfo().uuid.size());
   stream.write((ui64)callInfo().rpcId);
   mException.serialize(stream);
   stream.write(mMessage);
}

//-----------------------------------------------------------------------------
void RpcObject::pack(liber::netapp::SerialStream& stream)
{
  const_cast<const RpcObject*>(this)->pack(stream);
}

//-----------------------------------------------------------------------------
bool RpcObject::unpack(liber::netapp::SerialStream &stream)
{
  // Read the resource name
  if (stream.readCString(callInfo().resource) == SerialStream::ReadFail)
  {
    log::error("RpcObject::deserialize failure at %d\n", __LINE__);
    return false;
  }

  // Read the action name.
  if (stream.readCString(callInfo().action) == SerialStream::ReadFail)
  {
    log::error("RpcObject::deserialize failure at %d\n", __LINE__);
    return false;
  }

  if (stream.read((char*)callInfo().uuid.data, callInfo().uuid.size()) != SerialStream::ReadOk)
  {
    log::error("RpcObject::deserialize failure at %d\n", __LINE__);
    return false;
  }

  if (stream.read(callInfo().rpcId) == false)
  {
    log::error("RpcObject::deserialize failure at %d\n", __LINE__);
    return false;
  }

  // Read the exception.
  if (exception().deserialize(stream) == false)
  {
    log::error("RpcObject::deserialize failure at %d\n", __LINE__);
    return false;
  }

  // Read the protobuf message.
  if (stream.read(mMessage) == SerialStream::ReadFail)
  {
    log::error("RpcObject::deserialize failure at %d\n", __LINE__);
    return false;
  }

  return true;
}

