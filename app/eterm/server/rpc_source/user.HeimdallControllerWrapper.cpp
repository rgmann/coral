#include "HeimdallControllerWrapper.h"

using namespace liber::rpc;
using namespace rpc_eterm;
using namespace eterm;

//-----------------------------------------------------------------------------
void HeimdallControllerWrapper::
setLedState(const HcSetLedState& request, Status& response, RpcException& e)
{
   /* TODO: Implement action here and remove the following two lines. */
//   e.id = UnimplementedAction;
//   e.message = "HeimdallControllerWrapper::setLedState has not been implemented.";

   if (mpController)
   {
     HeimdallController::Status lStatus;
     lStatus = mpController->setLedState((LedId)request.led_id(), request.led_on());
     response.set_connected(lStatus.connected);
     response.set_busy(lStatus.busy);
   }
}


//-----------------------------------------------------------------------------
void HeimdallControllerWrapper::
activateDoor(const EmptyParams& request, Status& response, RpcException& e)
{
   /* TODO: Implement action here and remove the following two lines. */
//   e.id = UnimplementedAction;
//   e.message = "HeimdallControllerWrapper::activateDoor has not been implemented.";

   if (mpController)
   {
     HeimdallController::Status lStatus;
     lStatus = mpController->activateDoor();
     response.set_connected(lStatus.connected);
     response.set_busy(lStatus.busy);
   }
}


//-----------------------------------------------------------------------------
void HeimdallControllerWrapper::
enroll(const UserName& request, Status& response, RpcException& e)
{
   /* TODO: Implement action here and remove the following two lines. */
//   e.id = UnimplementedAction;
//   e.message = "HeimdallControllerWrapper::enroll has not been implemented.";

   if (mpController)
   {
     HeimdallController::Status lStatus;
     lStatus = mpController->enroll(request.first_name(), request.last_name());
     response.set_connected(lStatus.connected);
     response.set_busy(lStatus.busy);
   }
}


//-----------------------------------------------------------------------------
void HeimdallControllerWrapper::
getUsers(const EmptyParams& request, UserList& response, RpcException& e)
{
   /* TODO: Implement action here and remove the following two lines. */
//   e.id = UnimplementedAction;
//   e.message = "HeimdallControllerWrapper::getUsers has not been implemented.";

   if (mpController)
   {
     mpController->db().all(response);
   }
}


//-----------------------------------------------------------------------------
void HeimdallControllerWrapper::
removeOne(const UserID& request, Status& response, RpcException& e)
{
   /* TODO: Implement action here and remove the following two lines. */
//   e.id = UnimplementedAction;
//   e.message = "HeimdallControllerWrapper::removeOne has not been implemented.";

   if (mpController)
   {
     HeimdallController::Status lStatus;
     lStatus = mpController->removeOne(request.user_id());
     response.set_connected(lStatus.connected);
     response.set_busy(lStatus.busy);
   }
}


//-----------------------------------------------------------------------------
void HeimdallControllerWrapper::
removeAll(const EmptyParams& request, Status& response, RpcException& e)
{
   /* TODO: Implement action here and remove the following two lines. */
//   e.id = UnimplementedAction;
//   e.message = "HeimdallControllerWrapper::removeAll has not been implemented.";

   if (mpController)
   {
     HeimdallController::Status lStatus;
     lStatus = mpController->removeAll();
     response.set_connected(lStatus.connected);
     response.set_busy(lStatus.busy);
   }
}



