#include "AsyncRpcSupervisor.h"
#include "Log.h"

using namespace liber;
using namespace liber::rpc;
using namespace liber::concurrency;

//----------------------------------------------------------------------------
AsyncRpcSupervisor::AsyncRpcSupervisor()
: RpcSupervisor()
, IThread("AsyncRpcSupervisor")
{
  reset();
}

//----------------------------------------------------------------------------
AsyncRpcSupervisor::~AsyncRpcSupervisor()
{
}

//----------------------------------------------------------------------------
bool AsyncRpcSupervisor::isBusy()
{
  mCallMutex.lock();
  bool lbIsBusy = (mpCall != NULL);
  mCallMutex.unlock();
  return lbIsBusy;
}

//----------------------------------------------------------------------------
void AsyncRpcSupervisor::cancel()
{
  mCallMutex.lock();
  if (mpCall)
  {
    mpCall->cancel();
  }
  mCallMutex.unlock();
}

//----------------------------------------------------------------------------
bool AsyncRpcSupervisor::cancelled() const
{
  return mbCancelled;
}

//----------------------------------------------------------------------------
bool AsyncRpcSupervisor::reset()
{
  bool lbIsBusy = isBusy();

  if (!lbIsBusy)
  {
    mbCancelled = false;

    mCallMutex.lock();
    mpCall = NULL;
    mCallMutex.unlock();

    mpResponseMessage = NULL;

    mException.reset();
  }

  return (lbIsBusy == false);
}

//----------------------------------------------------------------------------
bool AsyncRpcSupervisor::failed() const
{
  return (mException.id != NoException);
}

//----------------------------------------------------------------------------
bool AsyncRpcSupervisor::
invoke(RpcClient& rClient, const RpcObject& request, PbMessage* pResponseMessage, int nTimeoutMs)
{
  bool lbSuccess = false;

  mException.pushFrame(TraceFrame("AsyncRpcSupervisor", "invoke",
                       __FILE__, __LINE__));

  if (isBusy() == false)
  {
    mnTimeoutMs = nTimeoutMs;
    mpResponseMessage = pResponseMessage;

    // Send the marshalled RPC to the RpcClient.
    mpCall = rClient.invokeRpc(request);
    lbSuccess = (mpCall != NULL);
    lbSuccess = (lbSuccess && launch());
  }

  mException.popFrame();

  return lbSuccess;
}

//----------------------------------------------------------------------------
void AsyncRpcSupervisor::run(const bool& bShutdown)
{
  mException.pushFrame(TraceFrame("AsyncRpcSupervisor", "run",
                       __FILE__, __LINE__));

  if (mpCall->wait(mnTimeoutMs))
  {
    if (mpCall->cancelled())
    {
      mException.reporter = RpcException::Client;
      mException.id       = RpcCallTimeout;
      mException.message  = "Call was cancelled before a response was received.";
    }
    else
    {
      mpCall->getResult(mResponseObject);
      liber::log::status("AsyncRpcSupervisor::run: %s\n", mResponseObject.exception().toString().c_str());

      if (mResponseObject.exception().id == NoException)
      {
        if (mpResponseMessage)
        {
          mResponseObject.getParams(*mpResponseMessage);
        }
      }
      else
      {
        mException.pushTrace(mResponseObject.exception());
      }
    }
  }
  else
  {
    mException.reporter = RpcException::Client;
    mException.id       = RpcCallTimeout;
    mException.message  = "Time out elapsed waiting for resource response.";
  }

  mpCall->dispose();

  mCallMutex.lock();
  mbCancelled = mpCall->cancelled();
  mpCall = NULL;
  mCallMutex.unlock();

  mException.popFrame();

  callback();
}

