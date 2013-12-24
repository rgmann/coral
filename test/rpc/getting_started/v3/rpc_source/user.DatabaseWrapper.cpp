#include "DatabaseWrapper.h"

using namespace liber::rpc;
using namespace rpc_getting_started;

//-----------------------------------------------------------------------------
RpcError DatabaseWrapper::
create(const UserAttributes& request, DbStatus& response, RpcException& e)
{
   /* TODO: Implement action here. */
   response.set_success( mDatabase.create( request.name().first(),
                                           request.name().last(),
                                           request));
}


//-----------------------------------------------------------------------------
RpcError DatabaseWrapper::
read(const UserName& request, UserAttributesList& response, RpcException& e)
{
   /* TODO: Implement action here. */
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
RpcError DatabaseWrapper::
update(const UserAttributes& request, DbStatus& response, RpcException& e)
{
   /* TODO: Implement action here. */
   response.set_success( mDatabase.update( request.name().first(),
                                           request.name().last(),
                                           request));
}


//-----------------------------------------------------------------------------
RpcError DatabaseWrapper::
destroy(const UserName& request, DbStatus& response, RpcException& e)
{
   /* TODO: Implement action here. */
   response.set_success( mDatabase.destroy( request.first(),
                                            request.last()));
}



