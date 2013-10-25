#include "DatabaseWrapper.h"
#include "DatabasecreateParamListParamList.h"
#include "DatabasereadParamListParamList.h"
#include "DatabaseupdateParamListParamList.h"
#include "DatabasedestroyParamListParamList.h"

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
void DatabaseWrapper::create(InstanceWrapper *pInst,
                                               Structure       &params, 
                                               RpcReturnValue  &retobj)
{
   bool lRetval /* = TODO: If applicable, set default */;

   reinterpret_cast<DatabasecreateParamList&>(params).get();

   /* TODO: Perform action. */

   retobj.set(lRetval);
}

//-----------------------------------------------------------------------------
void DatabaseWrapper::read(InstanceWrapper *pInst,
                                               Structure       &params, 
                                               RpcReturnValue  &retobj)
{
   bool lRetval /* = TODO: If applicable, set default */;

   reinterpret_cast<DatabasereadParamList&>(params).get();

   /* TODO: Perform action. */

   retobj.set(lRetval);
}

//-----------------------------------------------------------------------------
void DatabaseWrapper::update(InstanceWrapper *pInst,
                                               Structure       &params, 
                                               RpcReturnValue  &retobj)
{
   bool lRetval /* = TODO: If applicable, set default */;

   reinterpret_cast<DatabaseupdateParamList&>(params).get();

   /* TODO: Perform action. */

   retobj.set(lRetval);
}

//-----------------------------------------------------------------------------
void DatabaseWrapper::destroy(InstanceWrapper *pInst,
                                               Structure       &params, 
                                               RpcReturnValue  &retobj)
{
   bool lRetval /* = TODO: If applicable, set default */;

   reinterpret_cast<DatabasedestroyParamList&>(params).get();

   /* TODO: Perform action. */

   retobj.set(lRetval);
}


