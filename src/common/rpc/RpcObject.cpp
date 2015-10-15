// 
// Copyright (c) 2015, Robert Glissmann
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// 
// * Redistributions of source code must retain the above copyright notice, this
// list of conditions and the following disclaimer.
// 
// * Redistributions in binary form must reproduce the above copyright notice,
// this list of conditions and the following disclaimer in the documentation
// and/or other materials provided with the distribution.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// 

// %% license-end-token %%
// 
// Author: Robert.Glissmann@gmail.com (Robert Glissmann)
// 
// 



#include <string>
#include <iostream>
#include <sstream>
#include <boost/uuid/random_generator.hpp>
#include "Log.h"
#include "RpcObject.h"

using namespace coral;
using namespace coral::rpc;
using namespace coral::netapp;

//-----------------------------------------------------------------------------
RpcObject::RpcObject()
{
  mCallInfo.uuid = boost::uuids::random_generator()();
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
void RpcObject::pack(coral::netapp::SerialStream& stream) const
{
   stream.writeCString(callInfo().resource);
   stream.writeCString(callInfo().action);

   stream.write((const char*)callInfo().uuid.data, callInfo().uuid.size());
   // stream.write((ui64)callInfo().rpcId);
   mException.serialize(stream);
   stream.write(mMessage);
}

//-----------------------------------------------------------------------------
void RpcObject::pack(coral::netapp::SerialStream& stream)
{
  const_cast<const RpcObject*>(this)->pack(stream);
}

//-----------------------------------------------------------------------------
bool RpcObject::unpack(coral::netapp::SerialStream &stream)
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

  // if (stream.read(callInfo().rpcId) == false)
  // {
  //   log::error("RpcObject::deserialize failure at %d\n", __LINE__);
  //   return false;
  // }

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

