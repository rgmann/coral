#include "DatabaseWrapper.h"
#include "DatabaseCreateInput.h"
#include "DatabaseCreateOutput.h"
#include "DatabaseReadInput.h"
#include "DatabaseReadOutput.h"
#include "DatabaseUpdateInput.h"
#include "DatabaseUpdateOutput.h"
#include "DatabaseDestroyInput.h"
#include "DatabaseDestroyOutput.h"

//-----------------------------------------------------------------------------
DatabaseWrapper::DatabaseWrapper()
:  InstanceWrapper(),
   mpInst(NULL)
{
}

//-----------------------------------------------------------------------------
DatabaseWrapper::~DatabaseWrapper()
{
}

//-----------------------------------------------------------------------------
bool DatabaseWrapper::initialize(const Structure &params)
{
   mpInst = new Database(/* TODO: Pass correct parameters*/);
   return (mpInst != NULL);
}

//-----------------------------------------------------------------------------
bool DatabaseWrapper::destroy(const Structure &params)
{
   if (mpInst)
   {
      delete mpInst;
      mpInst = NULL;
   }
   return true;
}

//-----------------------------------------------------------------------------
RpcException DatabaseWrapper::
createDelegate
(InstanceWrapper* pInst, const Structure& input, Structure& output)
{
   DatabaseWrapper* lpInst = dynamic_cast<DatabaseWrapper*>(pInst);
   bool lReturnValue /* = TODO: If applicable, set default */;
   std::string lastname = "";
   UserAttributes attrs;

   DatabaseCreateInput inParams(input);
   if (!inParams.unmarshall(lastname, attrs))
   {
      return MissingParameters;
   }

   lReturnValue = lpInst->create(lastname, attrs);

   DatabaseCreateOutput outParams;
   outParams.setReturnValue(lReturnValue);
   output = outParams.getParameters();

   return NoException;
}


//-----------------------------------------------------------------------------
RpcException DatabaseWrapper::
readDelegate
(InstanceWrapper* pInst, const Structure& input, Structure& output)
{
   DatabaseWrapper* lpInst = dynamic_cast<DatabaseWrapper*>(pInst);
   bool lReturnValue /* = TODO: If applicable, set default */;
   std::string lastname = "";
   UserAttributes attrs;

   DatabaseReadInput inParams(input);
   if (!inParams.unmarshall(lastname, attrs))
   {
      return MissingParameters;
   }

   lReturnValue = lpInst->read(lastname, attrs);

   DatabaseReadOutput outParams;
   outParams.marshall(attrs);
   outParams.setReturnValue(lReturnValue);
   output = outParams.getParameters();

   return NoException;
}


//-----------------------------------------------------------------------------
RpcException DatabaseWrapper::
updateDelegate
(InstanceWrapper* pInst, const Structure& input, Structure& output)
{
   DatabaseWrapper* lpInst = dynamic_cast<DatabaseWrapper*>(pInst);
   bool lReturnValue /* = TODO: If applicable, set default */;
   std::string lastname = "";
   UserAttributes attrs;

   DatabaseUpdateInput inParams(input);
   if (!inParams.unmarshall(lastname, attrs))
   {
      return MissingParameters;
   }

   lReturnValue = lpInst->update(lastname, attrs);

   DatabaseUpdateOutput outParams;
   outParams.setReturnValue(lReturnValue);
   output = outParams.getParameters();

   return NoException;
}


//-----------------------------------------------------------------------------
RpcException DatabaseWrapper::
destroyDelegate
(InstanceWrapper* pInst, const Structure& input, Structure& output)
{
   DatabaseWrapper* lpInst = dynamic_cast<DatabaseWrapper*>(pInst);
   bool lReturnValue /* = TODO: If applicable, set default */;
   std::string lastname = "";

   DatabaseDestroyInput inParams(input);
   if (!inParams.unmarshall(lastname))
   {
      return MissingParameters;
   }

   lReturnValue = lpInst->destroy(lastname);

   DatabaseDestroyOutput outParams;
   outParams.setReturnValue(lReturnValue);
   output = outParams.getParameters();

   return NoException;
}



