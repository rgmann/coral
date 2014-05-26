#include "rpc_calculator_app.pb.h"
#include "DatabaseClientStub.h"

using namespace liber::rpc;
using namespace rpc_calculator_app;

//------------------------------------------------------------------------------
DatabaseClientStub::DatabaseClientStub(RpcClient &client)
: RpcClientResource(client, "Database")
{
}

//------------------------------------------------------------------------------
DatabaseClientStub::~DatabaseClientStub()
{
}

//------------------------------------------------------------------------------
void DatabaseClientStub::create(const UserAttributes& request, DbStatus& response) throw (RpcException)
{
   if (!call("create", request, response))
   {
      throw getLastError();
   }
}


//------------------------------------------------------------------------------
void DatabaseClientStub::read(const UserName& request, UserAttributesList& response) throw (RpcException)
{
   if (!call("read", request, response))
   {
      throw getLastError();
   }
}


//------------------------------------------------------------------------------
void DatabaseClientStub::update(const UserAttributes& request, DbStatus& response) throw (RpcException)
{
   if (!call("update", request, response))
   {
      throw getLastError();
   }
}


//------------------------------------------------------------------------------
void DatabaseClientStub::destroy(const UserName& request, DbStatus& response) throw (RpcException)
{
   if (!call("destroy", request, response))
   {
      throw getLastError();
   }
}



