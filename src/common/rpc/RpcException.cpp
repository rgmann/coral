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
#include <iomanip>
#include <boost/uuid/nil_generator.hpp>
#include "Log.h"
#include "RpcException.h"

using namespace coral;
using namespace coral::rpc;
using namespace coral::netapp;

//-----------------------------------------------------------------------------
TraceFrame::TraceFrame(const std::string& className,
                       const std::string& methodName,
                       const std::string& filename,
                       int lineNumber)
: mClassName(className),
  mMethodName(methodName),
  mFileName(filename),
  mLineNumber(lineNumber)
{
}

//-----------------------------------------------------------------------------
std::string TraceFrame::toString() const
{
   std::stringstream ss;

   ss << "from " 
      << mFileName << ":" 
      << mLineNumber << ":in "
      << mClassName << "."
      << mMethodName;

   return ss.str();
}

//-----------------------------------------------------------------------------
void TraceFrame::pack(coral::netapp::SerialStream& stream) const
{
   stream.writeCString(mClassName);
   stream.writeCString(mMethodName);
   stream.writeCString(mFileName);
   stream.write((ui32)mLineNumber);
}

//-----------------------------------------------------------------------------
void TraceFrame::pack(coral::netapp::SerialStream& stream)
{
  const_cast<const TraceFrame*>(this)->pack(stream);
}

//-----------------------------------------------------------------------------
bool TraceFrame::unpack(coral::netapp::SerialStream& stream)
{
   if (stream.readCString(mClassName) == coral::netapp::SerialStream::ReadFail)
   {
      log::error("TraceFrame::deserialize failure at %d\n", __LINE__);
      return false;
   }
   if (stream.readCString(mMethodName) == coral::netapp::SerialStream::ReadFail)
   {
      log::error("TraceFrame::deserialize failure at %d\n", __LINE__);
      return false;
   }
   if (stream.readCString(mFileName) == coral::netapp::SerialStream::ReadFail)
   {
      log::error("TraceFrame::deserialize failure at %d\n", __LINE__);
      return false;
   }
   if (stream.read(mLineNumber) == false)
   {
      log::error("TraceFrame::deserialize failure at %d\n", __LINE__);
      return false;
   }
   return true;
}

//-----------------------------------------------------------------------------
RpcException::RpcException()
: id(NoException),
  reporter(RpcException::Unknown)
{
}

//-----------------------------------------------------------------------------
const RpcCallInfo& RpcException::callInfo() const
{
   return mCallInfo;
}

//-----------------------------------------------------------------------------
std::string RpcException::toString() const
{
   std::string lErrorString = "";

   lErrorString += callInfo().resource + "(";
   switch (reporter)
   {
   case Server: lErrorString += "Server)"; break;
   case Client: lErrorString += "Client)"; break;
   default: lErrorString += "Unknown)"; break;
   }

   lErrorString += "::" + callInfo().action + ": ";
   lErrorString += ToRpcErrorString(id);

   if (message.length() > 0)
   {
      lErrorString += " > " + message;
   }

   return lErrorString;
}

//-----------------------------------------------------------------------------
void RpcException::reset()
{
  id = NoException;
  message = "";

  mCallInfo.resource = "";
  mCallInfo.action = "";
  mCallInfo.uuid = boost::uuids::nil_generator()();
  // mCallInfo.rpcId = 0;

  mTrace.clear();
}

//-----------------------------------------------------------------------------
void RpcException::pack(coral::netapp::SerialStream& stream) const
{
   stream.write((ui32)reporter);
   stream.write((ui32)id);
   stream.writeCString(message);

   // Serialize the frame trace only if an exception occurred.
   if (id != NoException)
   {
      stream.write((ui8)mTrace.size());

      std::vector<TraceFrame>::const_iterator lIt = mTrace.begin();
      for (; lIt != mTrace.end(); lIt++)
      {
         lIt->serialize(stream);
      }
   }
   else
   {
      stream.write((ui8)0);
   }
}

//-----------------------------------------------------------------------------
void RpcException::pack(coral::netapp::SerialStream& stream)
{
  const_cast<const RpcException*>(this)->pack(stream);
}

//-----------------------------------------------------------------------------
bool RpcException::unpack(coral::netapp::SerialStream& stream)
{
   i32 tI32Field = -1;
   if (stream.read(tI32Field) == false)
   {
      log::error("RpcException::deserialize failure at %d\n", __LINE__);
      return false;
   }
   reporter = (Reporter)tI32Field;

   if (stream.read(tI32Field) == false)
   {
      log::error("RpcException::deserialize failure at %d\n", __LINE__);
      return false;
   }
   id = (RpcErrorId)tI32Field;

   if (stream.readCString(message) == coral::netapp::SerialStream::ReadFail)
   {
      log::error("RpcException::deserialize failure at %d\n", __LINE__);
      return false;
   }

   ui8 lnFrameCount = 0;
   if (stream.read(lnFrameCount) == false)
   {
      log::error("RpcException::deserialize failure at %d\n", __LINE__);
      return false;
   }
   for (ui8 lnFrameInd = 0; lnFrameInd < lnFrameCount; lnFrameInd++)
   {
      TraceFrame lFrame;
      if (lFrame.deserialize(stream) == false)
      {
         log::error("RpcException::deserialize failure at %d\n", __LINE__);
         return false;
      }

      mTrace.push_back(lFrame);
   } 

   return true;
}

//-----------------------------------------------------------------------------
void RpcException::pushFrame(const TraceFrame& frame)
{
   mTrace.push_back(frame);
}

//-----------------------------------------------------------------------------
bool RpcException::pushTrace(const RpcException& other)
{
  bool lbSuccess = false;

  if (id == NoException)
  {
    id = other.id;
    message = other.message;
    mCallInfo = other.mCallInfo;

    std::vector<TraceFrame>::const_iterator lIt = other.frameTrace().begin();
    for (; lIt != other.frameTrace().end(); lIt++) pushFrame(*lIt);
  }

  return lbSuccess;
}

//-----------------------------------------------------------------------------
void RpcException::popFrame() 
{
   if (id == NoException)
   {
     mTrace.pop_back();
   }
}

//-----------------------------------------------------------------------------
const std::vector<TraceFrame>& RpcException::frameTrace() const
{
   return mTrace;
}

//-----------------------------------------------------------------------------
std::vector<std::string> RpcException::textTrace() const 
{
   std::vector<std::string> lTraceVec;
   if (id != NoException)
   {
      std::vector<TraceFrame>::const_reverse_iterator lIt = mTrace.rbegin();
      for (; lIt != mTrace.rend(); lIt++) lTraceVec.push_back(lIt->toString());
   }

   return lTraceVec;
}

//-----------------------------------------------------------------------------
std::string RpcException::trace() const
{
   std::stringstream lStream;
   if (id == NoException) return "";

   std::vector<std::string> lvTrace = textTrace();
   std::vector<std::string>::iterator lIt = lvTrace.begin();

   lStream << "RpcException::" + ToRpcErrorString(id) + ": " + message + " from:" << std::endl;
   for (; lIt != lvTrace.end(); lIt++)
   {
      lStream << "  " << *lIt << std::endl;
   }

   return lStream.str();
}

