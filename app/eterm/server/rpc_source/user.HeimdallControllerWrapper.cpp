#include "HeimdallControllerWrapper.h"

using namespace liber::rpc;
using namespace rpc_eterm;

//-----------------------------------------------------------------------------
void HeimdallControllerWrapper::
setLedState(const LedState& request, Status& response, RpcException& e)
{
   /* TODO: Implement action here and remove the following two lines. */
   //e.id = UnimplementedAction;
   //e.message = "HeimdallControllerWrapper::setLedState has not been implemented.";
   eterm::HeimdallStatus::Data lStatus;
   eterm::HeimdallController::Status lClientStatus;
   lClientStatus = mController.setLedState((eterm::LedId)request.led_id(), request.led_on(), lStatus);
   response.set_success(lStatus.success);
   //response.set_is_connected(mController.isConnected());
   response.set_is_connected(lClientStatus.connected);
}


//-----------------------------------------------------------------------------
void HeimdallControllerWrapper::
getFingerprintStatus(const EmptyParams& request, Status& response, RpcException& e)
{
   /* TODO: Implement action here and remove the following two lines. */
   //e.id = UnimplementedAction;
   //e.message = "HeimdallControllerWrapper::getFingerprintStatus has not been implemented.";
   eterm::HeimdallStatus::Data lStatus;
   eterm::FingerprintStatus::Data lFpStatus;
   eterm::HeimdallController::Status lClientStatus;
   lClientStatus = mController.getFingerprintStatus(lStatus, lFpStatus);

   response.set_success(lStatus.success);
   //response.set_is_connected(mController.isConnected());
   response.set_is_connected(lClientStatus.connected);

   if (lClientStatus.connected && lClientStatus.success)
   {
     response.mutable_fp_status()->set_num_records(lFpStatus.numRecords);
     response.mutable_fp_status()->set_finger_pressed(lFpStatus.fingerPressed);
   }
}



