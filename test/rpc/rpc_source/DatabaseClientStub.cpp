#include "DatabaseClientStub.h"
#include "DatabaseCreateInput.h"
#include "DatabaseCreateOutput.h"
#include "DatabaseReadInput.h"
#include "DatabaseReadOutput.h"
#include "DatabaseUpdateInput.h"
#include "DatabaseUpdateOutput.h"
#include "DatabaseDestroyInput.h"
#include "DatabaseDestroyOutput.h"

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
bool DatabaseClientStub::create(std::string lastname, const UserAttributes& attrs) throw (RpcError)
{
   DatabaseCreateInput lInParams;
   Structure lOutStructure;
   bool lReturnValue;

   // Marshall the parameters.
   lInParams.marshall(lastname, attrs);

   if (call("create", lInParams.getParameters(), lOutStructure))
   {
      DatabaseCreateOutput lOutParams(lOutStructure);
      lOutParams.getReturnValue(lReturnValue);
   }
   else
   {
      throw getLastError();
   }

   return lReturnValue;
}


//------------------------------------------------------------------------------
bool DatabaseClientStub::read(std::string lastname, UserAttributes& attrs) throw (RpcError)
{
   DatabaseReadInput lInParams;
   Structure lOutStructure;
   bool lReturnValue;

   // Marshall the parameters.
   lInParams.marshall(lastname, attrs);

   if (call("read", lInParams.getParameters(), lOutStructure))
   {
      DatabaseReadOutput lOutParams(lOutStructure);
      lOutParams.unmarshall(attrs);
      lOutParams.getReturnValue(lReturnValue);
   }
   else
   {
      throw getLastError();
   }

   return lReturnValue;
}


//------------------------------------------------------------------------------
bool DatabaseClientStub::update(std::string lastname, const UserAttributes& attrs) throw (RpcError)
{
   DatabaseUpdateInput lInParams;
   Structure lOutStructure;
   bool lReturnValue;

   // Marshall the parameters.
   lInParams.marshall(lastname, attrs);

   if (call("update", lInParams.getParameters(), lOutStructure))
   {
      DatabaseUpdateOutput lOutParams(lOutStructure);
      lOutParams.getReturnValue(lReturnValue);
   }
   else
   {
      throw getLastError();
   }

   return lReturnValue;
}


//------------------------------------------------------------------------------
bool DatabaseClientStub::destroy(std::string lastname) throw (RpcError)
{
   DatabaseDestroyInput lInParams;
   Structure lOutStructure;
   bool lReturnValue;

   // Marshall the parameters.
   lInParams.marshall(lastname);

   if (call("destroy", lInParams.getParameters(), lOutStructure))
   {
      DatabaseDestroyOutput lOutParams(lOutStructure);
      lOutParams.getReturnValue(lReturnValue);
   }
   else
   {
      throw getLastError();
   }

   return lReturnValue;
}



