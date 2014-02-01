#include "HeimdallControllerWrapper.h"

using namespace liber::rpc;
using namespace rpc_eterm;

//-----------------------------------------------------------------------------
void HeimdallControllerWrapper::
setLedState(const LedState& request, Status& response, RpcException& e)
{
   /* TODO: Implement action here and remove the following two lines. */
//   e.id = UnimplementedAction;
//   e.message = "HeimdallControllerWrapper::setLedState has not been implemented.";
  eterm::HeimdallStatus::Data lStatus;
  mController.setLedState(request.led_on(), lStatus);
  response.set_success(lStatus.success);
  response.set_is_connected(mController.isConnected());
}



