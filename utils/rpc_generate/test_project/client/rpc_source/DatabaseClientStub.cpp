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
bool DatabaseClientStub::
create(std::string lastname, const UserAttributes& attrs, bool& result)
{
   DatabaseCreateInput lInParams;
   Structure lOutStructure;
   bool lbSucces = false;

   // Marshall the parameters.
   lInParams.marshall(lastname, attrs);

   if (call("create", lInParams.getParameters(), lOutStructure))
   {
      DatabaseCreateOutput lOutParams(lOutStructure);
      lbSuccess &= lOutParams.getReturnValue(result);
   }

   return lbSuccess;
}


//------------------------------------------------------------------------------
bool DatabaseClientStub::
read(std::string lastname, UserAttributes& attrs, bool& result)
{
   DatabaseReadInput lInParams;
   Structure lOutStructure;
   bool lbSucces = false;

   // Marshall the parameters.
   lInParams.marshall(lastname, attrs);

   if (call("read", lInParams.getParameters(), lOutStructure))
   {
      DatabaseReadOutput lOutParams(lOutStructure);
      lbSuccess = lOutParams.unmarshall(attrs);
      lbSuccess &= lOutParams.getReturnValue(result);
   }

   return lbSuccess;
}


//------------------------------------------------------------------------------
bool DatabaseClientStub::
update(std::string lastname, const UserAttributes& attrs, bool& result)
{
   DatabaseUpdateInput lInParams;
   Structure lOutStructure;
   bool lbSucces = false;

   // Marshall the parameters.
   lInParams.marshall(lastname, attrs);

   if (call("update", lInParams.getParameters(), lOutStructure))
   {
      DatabaseUpdateOutput lOutParams(lOutStructure);
      lbSuccess &= lOutParams.getReturnValue(result);
   }

   return lbSuccess;
}


//------------------------------------------------------------------------------
bool DatabaseClientStub::
destroy(std::string lastname, bool& result)
{
   DatabaseDestroyInput lInParams;
   Structure lOutStructure;
   bool lbSucces = false;

   // Marshall the parameters.
   lInParams.marshall(lastname);

   if (call("destroy", lInParams.getParameters(), lOutStructure))
   {
      DatabaseDestroyOutput lOutParams(lOutStructure);
      lbSuccess &= lOutParams.getReturnValue(result);
   }

   return lbSuccess;
}



