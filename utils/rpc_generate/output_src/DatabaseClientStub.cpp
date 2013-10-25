#include "DatabaseClientStub.h"
#include "DatabasecreateParamListParamList.h"
#include "DatabasereadParamListParamList.h"
#include "DatabaseupdateParamListParamList.h"
#include "DatabasedestroyParamListParamList.h"

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
bool DatabaseClientStub::create(bool& result)
{
   DatabasecreateParamListParamList lParams;
   RpcReturnValue lCallRetVal;

   return (call("create", lParams, lCallRetVal) &&
               lCallRetVal.get(result));
}


//------------------------------------------------------------------------------
bool DatabaseClientStub::read(bool& result)
{
   DatabasereadParamListParamList lParams;
   RpcReturnValue lCallRetVal;

   return (call("read", lParams, lCallRetVal) &&
               lCallRetVal.get(result));
}


//------------------------------------------------------------------------------
bool DatabaseClientStub::update(bool& result)
{
   DatabaseupdateParamListParamList lParams;
   RpcReturnValue lCallRetVal;

   return (call("update", lParams, lCallRetVal) &&
               lCallRetVal.get(result));
}


//------------------------------------------------------------------------------
bool DatabaseClientStub::destroy(bool& result)
{
   DatabasedestroyParamListParamList lParams;
   RpcReturnValue lCallRetVal;

   return (call("destroy", lParams, lCallRetVal) &&
               lCallRetVal.get(result));
}



