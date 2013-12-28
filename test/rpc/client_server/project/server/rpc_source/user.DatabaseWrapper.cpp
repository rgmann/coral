#include <iostream>
#include "DatabaseWrapper.h"

using namespace liber::rpc;
using namespace rpc_getting_started;

//-----------------------------------------------------------------------------
void DatabaseWrapper::
create(const UserAttributes& request, DbStatus& response, RpcException& e)
{
   response.set_success( mDatabase.create( request.name().first(),
                                           request.name().last(),
                                           request));
}


//-----------------------------------------------------------------------------
void DatabaseWrapper::
read(const UserName& request, UserAttributesList& response, RpcException& e)
{
   std::vector<UserAttributes> users;
   bool lbSuccess = mDatabase.read( request.first(),
                                    request.last(),
                                    users);
   if (lbSuccess)
   {
      std::vector<UserAttributes>::iterator lIt = users.begin();
      for (; lIt != users.end(); lIt++)
      {
         response.add_users()->CopyFrom(*lIt);
      }
   }
}


//-----------------------------------------------------------------------------
void DatabaseWrapper::
update(const UserAttributes& request, DbStatus& response, RpcException& e)
{
   response.set_success( mDatabase.update( request.name().first(),
                                           request.name().last(),
                                           request));
}


//-----------------------------------------------------------------------------
void DatabaseWrapper::
destroy(const UserName& request, DbStatus& response, RpcException& e)
{
   response.set_success( mDatabase.destroy( request.first(),
                                            request.last()));
}



