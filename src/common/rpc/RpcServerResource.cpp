#include <utility>
#include <iostream>
#include <sstream>
#include <boost/uuid/random_generator.hpp>
#include "Log.h"
//#include "Md5Hash.h"
#include "Timestamp.h"
#include "RpcServerResource.h"

using namespace liber;
using namespace liber::rpc;

//------------------------------------------------------------------------------
RpcServerResource::RpcServerResource(const std::string &name)
:  mName(name),
   mnInstanceCount(0)
{
}

//------------------------------------------------------------------------------
std::string RpcServerResource::getName() const
{
   return mName;
}

//------------------------------------------------------------------------------
bool RpcServerResource::unmarshall(RpcObject &input, RpcObject &output)
{
   bool lbSuccess = true;
   
   boost::uuids::uuid lUuid = input.callInfo().uuid;

   input.exception().pushFrame(TraceFrame("RpcServerResource", "unmarshall",
                                          __FILE__, __LINE__));
   
   if (!input.isValid())
   {
      exception(input, output, MissingParameters);
      return false;
   }

   if (input.callInfo().action == "construct")
   {
      lbSuccess = construct(input, output);
   }
   else if (input.callInfo().action == "destroy")
   {
      lbSuccess = destroy(input, output);
   }
   else
   {
      if (isValidInstance(lUuid))
      {
         lbSuccess = invoke(lUuid, input, output);
      }
      else
      {
         exception(input, output, InvalidUIID, "Invalid instance UIID");
         lbSuccess = false;
      }
   }

   input.exception().popFrame();
   
   return lbSuccess;
}

//------------------------------------------------------------------------------
//bool RpcServerResource::isValidInstance(Md5Hash& uiid)
bool RpcServerResource::isValidInstance(boost::uuids::uuid& uuid)
{
   return (getInstance(uuid) != NULL);
}

//------------------------------------------------------------------------------
bool RpcServerResource::construct(RpcObject &input, RpcObject &output)
{
   bool lbSuccess = false;
   std::string lParamList;

   input.exception().pushFrame(TraceFrame("RpcServerResource", "construct",
                                           __FILE__, __LINE__));

   boost::uuids::uuid lUUID = boost::uuids::random_generator()();
   
   if (mInstances.count(lUUID) != 0)
   {
      exception(input, output, UIIDAssignmentErr);
      return false;
   }
   
   input.getParams(lParamList);

   InstanceWrapper* lpWrapper = createInstance();
   if (lpWrapper == NULL)
   {
      exception(input, output, NullInstance,
                "Failed to instantiate new \"" +
                input.callInfo().resource + "\" resource");
      return false;
   }

   mInstances.insert(std::make_pair(lUUID, lpWrapper));

   lbSuccess = lpWrapper->initialize(lParamList);
   tugCtorHook(lpWrapper);
 
   input.getResponse(output);
   output.callInfo().uuid = lUUID;

   mnInstanceCount++;

   input.exception().popFrame();

   return lbSuccess;
}

//------------------------------------------------------------------------------
bool RpcServerResource::invoke(boost::uuids::uuid&   uuid,
                               RpcObject& input,
                               RpcObject& output)
{
   bool lbSuccess = (mMethodMap.count(input.callInfo().action) != 0);

   input.exception().pushFrame(TraceFrame("RpcServerResource", "invoke",
                                           __FILE__, __LINE__));

   if (lbSuccess)
   {
      std::string lInParams;
      std::string lOutParams;

      InstanceWrapper::Method wrapper = NULL;
      input.getParams(lInParams);

      wrapper = mMethodMap.find(input.callInfo().action)->second;
      if (wrapper)
      {
         wrapper(getInstance(uuid), lInParams, lOutParams, input.exception());
         input.getResponse(output, lOutParams);
      }
      else
      {
         exception(input, output, NullInstance,
                   "Null reference for \"" + 
                   input.callInfo().action + "\" action");
         lbSuccess = false;
      }
   }
   else
   {
      exception(input, output, UnknownMethod,
                "Unrecognized action \"" + input.callInfo().action + "\"");
   }

   input.exception().popFrame();
   
   return lbSuccess;
}

//------------------------------------------------------------------------------
bool RpcServerResource::destroy(RpcObject &input, RpcObject &output)
{
   input.exception().pushFrame(TraceFrame("RpcServerResource", "destroy",
                                           __FILE__, __LINE__));

   boost::uuids::uuid lUuid = input.callInfo().uuid;
   if (mInstances.count(lUuid) == 0)
   {
      exception(input, output, InvalidUIID, "Invalid instance UIID");
      return false;
   }

   std::string lInParams;
   input.getParams(lInParams);

   InstanceWrapper* lpWrapper = mInstances.find(lUuid)->second;

   if (lpWrapper == NULL)
   {
      exception(input, output, NullInstance, "Null wrapper reference");
      return false;
   }

   tugDtorHook(lpWrapper);
   lpWrapper->destroy(lInParams);

   delete lpWrapper;
   mInstances.erase(lUuid);
   
   input.getResponse(output);
   mnInstanceCount--;

   input.exception().popFrame();
   
   return true;
}

//------------------------------------------------------------------------------
InstanceWrapper* RpcServerResource::getInstance(boost::uuids::uuid& uuid)
{
   InstanceWrapper* lpInstance = NULL;
   
   if (mInstances.count(uuid) != 0)
   {
      lpInstance = mInstances.find(uuid)->second;
   }
   
   return lpInstance;
}

//------------------------------------------------------------------------------
bool RpcServerResource::addAction(const std::string &actionName,
                                  InstanceWrapper::Method method)
{
   bool lbMethodExists = (mMethodMap.count(actionName) != 0);
   
   if (!lbMethodExists)
   {
      mMethodMap.insert(std::make_pair(actionName, method));
   }
   
   return lbMethodExists;
}

//------------------------------------------------------------------------------
void RpcServerResource::exception(RpcObject&         request,
                                  RpcObject&         response,
                                  RpcErrorId         eid,
                                  const std::string& message)
{
   request.exception().reporter = RpcException::Server;
   request.exception().id       = eid;
   request.exception().message  = message;

   request.getResponse(response);
}

//------------------------------------------------------------------------------
/*void RpcServerResource::createUIID(Md5Hash& hash)
{
   std::stringstream stream;

   ++mnCurrentInstId;
   stream << mName << Timestamp::Now().toMicroseconds() << mnCurrentInstId;
   std::string lUiidBlock = stream.str();

   hash.hashify((ui8*)lUiidBlock.data(), lUiidBlock.size());
}*/

