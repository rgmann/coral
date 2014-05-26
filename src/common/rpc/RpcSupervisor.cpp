#include "RpcSupervisor.h"

using namespace liber::rpc;

//----------------------------------------------------------------------------
RpcException& RpcSupervisor::exception()
{
  return mException;
}

//----------------------------------------------------------------------------
RpcObject& RpcSupervisor::response()
{
  return mResponseObject;
}

