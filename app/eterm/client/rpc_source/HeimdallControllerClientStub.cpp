#include "rpc_eterm.pb.h"
#include "HeimdallControllerClientStub.h"

using namespace liber::rpc;
using namespace rpc_eterm;

//------------------------------------------------------------------------------
HeimdallControllerClientStub::HeimdallControllerClientStub(RpcClient &client)
: RpcClientResource(client, "HeimdallController")
{
}

//------------------------------------------------------------------------------
HeimdallControllerClientStub::~HeimdallControllerClientStub()
{
}

//------------------------------------------------------------------------------
void HeimdallControllerClientStub::setLedState(const HcSetLedState& request, Status& response) throw (RpcException)
{
   if (!call("setLedState", request, response))
   {
      throw getLastError();
   }
}


//------------------------------------------------------------------------------
void HeimdallControllerClientStub::activateDoor(const EmptyParams& request, Status& response) throw (RpcException)
{
   if (!call("activateDoor", request, response))
   {
      throw getLastError();
   }
}


//------------------------------------------------------------------------------
void HeimdallControllerClientStub::enroll(const UserName& request, Status& response) throw (RpcException)
{
   if (!call("enroll", request, response))
   {
      throw getLastError();
   }
}


//------------------------------------------------------------------------------
void HeimdallControllerClientStub::getUsers(const EmptyParams& request, UserList& response) throw (RpcException)
{
   if (!call("getUsers", request, response))
   {
      throw getLastError();
   }
}


//------------------------------------------------------------------------------
void HeimdallControllerClientStub::removeOne(const UserID& request, Status& response) throw (RpcException)
{
   if (!call("removeOne", request, response))
   {
      throw getLastError();
   }
}


//------------------------------------------------------------------------------
void HeimdallControllerClientStub::removeAll(const EmptyParams& request, Status& response) throw (RpcException)
{
   if (!call("removeAll", request, response))
   {
      throw getLastError();
   }
}



