#include "DatabaseClientStub.h"
#include "DatabasecreateParamListParamList.h"
#include "DatabasereadParamListParamList.h"
#include "DatabaseupdateParamListParamList.h"
#include "DatabasedeleteParamListParamList.h"

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
bool DatabaseClientStub::delete(bool& result)
{
   DatabasedeleteParamListParamList lParams;
   RpcReturnValue lCallRetVal;

   return (call("delete", lParams, lCallRetVal) &&
               lCallRetVal.get(result));
}



