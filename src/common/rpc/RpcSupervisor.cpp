#include "RpcSupervisor.h"

using namespace liber::rpc;

//----------------------------------------------------------------------------
RpcException& RpcSupervisor::exception()
{
  return exception_;
}

//----------------------------------------------------------------------------
RpcObject& RpcSupervisor::response()
{
  return response_object_;
}

