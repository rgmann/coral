#include "DatabaseWrapper.h"
#include "DatabasecreateParamListParamList.h"
#include "DatabasereadParamListParamList.h"
#include "DatabaseupdateParamListParamList.h"
#include "DatabasedeleteParamListParamList.h"

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

   reinterpret_cast<DatabasecreateParamListParamList&>(params).get();

   /* TODO: Perform action. */

   retobj.set(lRetval);
}

//-----------------------------------------------------------------------------
void DatabaseWrapper::read(InstanceWrapper *pInst,
                                               Structure       &params, 
                                               RpcReturnValue  &retobj)
{
   bool lRetval /* = TODO: If applicable, set default */;

   reinterpret_cast<DatabasereadParamListParamList&>(params).get();

   /* TODO: Perform action. */

   retobj.set(lRetval);
}

//-----------------------------------------------------------------------------
void DatabaseWrapper::update(InstanceWrapper *pInst,
                                               Structure       &params, 
                                               RpcReturnValue  &retobj)
{
   bool lRetval /* = TODO: If applicable, set default */;

   reinterpret_cast<DatabaseupdateParamListParamList&>(params).get();

   /* TODO: Perform action. */

   retobj.set(lRetval);
}

//-----------------------------------------------------------------------------
void DatabaseWrapper::delete(InstanceWrapper *pInst,
                                               Structure       &params, 
                                               RpcReturnValue  &retobj)
{
   bool lRetval /* = TODO: If applicable, set default */;

   reinterpret_cast<DatabasedeleteParamListParamList&>(params).get();

   /* TODO: Perform action. */

   retobj.set(lRetval);
}


