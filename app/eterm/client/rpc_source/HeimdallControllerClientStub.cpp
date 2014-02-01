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
void HeimdallControllerClientStub::setLedState(const LedState& request, Status& response) throw (RpcException)
{
   if (!call("setLedState", request, response))
   {
      throw getLastError();
   }
}


//------------------------------------------------------------------------------
void HeimdallControllerClientStub::getFingerprintStatus(const EmptyParams& request, Status& response) throw (RpcException)
{
   if (!call("getFingerprintStatus", request, response))
   {
      throw getLastError();
   }
}



