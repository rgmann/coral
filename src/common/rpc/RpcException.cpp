#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>
#include "PacketHelper.h"
#include "RpcException.h"

using namespace liber::rpc;
using namespace packethelper;

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
std::string TraceFrame::serialize() const
{
   PacketCtor lPacket(packethelper::NetworkByteOrder);
   lPacket.writeCString(mClassName);
   lPacket.writeCString(mMethodName);
   lPacket.writeCString(mFileName);
   lPacket.write((ui32)mLineNumber);
   return lPacket.stream.str(); 
}

//-----------------------------------------------------------------------------
bool TraceFrame::deserialize(const std::string& data)
{
   PacketDtor lPacket(packethelper::NetworkByteOrder);
   lPacket.setData(data);
   if (lPacket.readCString(mClassName) == PacketDtor::ReadFail)
   {
      std::cout << "TraceFrame::deserialize failure at " << __LINE__ << std::endl;
      return false;
   }
   if (lPacket.readCString(mMethodName) == PacketDtor::ReadFail)
   {
      std::cout << "TraceFrame::deserialize failure at " << __LINE__ << std::endl;
      return false;
   }
   if (lPacket.readCString(mFileName) == PacketDtor::ReadFail)
   {
      std::cout << "TraceFrame::deserialize failure at " << __LINE__ << std::endl;
      return false;
   }
   if (!lPacket.read(mLineNumber))
   {
      std::cout << "TraceFrame::deserialize failure at " << __LINE__ << std::endl;
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
std::string RpcException::serialize() const
{
   PacketCtor lPacket(packethelper::NetworkByteOrder);

   lPacket.write((ui32)reporter);
   lPacket.write((ui32)id);
   lPacket.writeCString(message);

   // Serialize the frame trace only if an exception occurred.
   if (id != NoException)
   {
      lPacket.write((ui8)mTrace.size());

      std::vector<TraceFrame>::const_iterator lIt = mTrace.begin();
      for (; lIt != mTrace.end(); lIt++)
      {
         lPacket.write(lIt->serialize());
      }
   }
   else
   {
      lPacket.write((ui8)0);
   }

   return lPacket.stream.str();
}

//-----------------------------------------------------------------------------
bool RpcException::deserialize(const std::string& data)
{
   PacketDtor lPacket(packethelper::NetworkByteOrder);
   lPacket.setData(data);

   i32 tI32Field = -1;
   if (!lPacket.read(tI32Field))
   {
      std::cout << "RpcException::deserialize failure at " << __LINE__ << std::endl;
      return false;
   }
   reporter = (Reporter)tI32Field;

   if (!lPacket.read(tI32Field))
   {
      std::cout << "RpcException::deserialize failure at " << __LINE__ << std::endl;
      return false;
   }
   id = (RpcErrorId)tI32Field;

   if (lPacket.readCString(message) == PacketDtor::ReadFail)
   {
      std::cout << "RpcException::deserialize failure at " << __LINE__ << std::endl;
      return false;
   }

   ui8 lnFrameCount = 0;
   if (!lPacket.read(lnFrameCount))
   {
      std::cout << "RpcException::deserialize failure at " << __LINE__ << std::endl;
      return false;
   }
   for (ui8 lnFrameInd = 0; lnFrameInd < lnFrameCount; lnFrameInd++)
   {
      std::string lFrameData;
      TraceFrame lFrame;
      if (lPacket.read(lFrameData) == PacketDtor::ReadFail)
      {
         std::cout << "RpcException::deserialize failure at " << __LINE__ << std::endl;
         return false;
      }
      if (!lFrame.deserialize(lFrameData)) return false;
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
void RpcException::popFrame() 
{
   mTrace.pop_back();
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

