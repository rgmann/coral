#ifndef ASYNC_RPC_SUPERVISOR_H
#define ASYNC_RPC_SUPERVISOR_H

#include <boost/thread/mutex.hpp>
#include "IThread.h"
#include "RpcSupervisor.h"

namespace liber {
namespace rpc {

class AsyncRpcSupervisor : public RpcSupervisor, public liber::concurrency::IThread {
public:

  AsyncRpcSupervisor();
  virtual ~AsyncRpcSupervisor();

  void cancel();
  bool cancelled() const;

  bool isBusy();

  bool reset();

  bool failed() const;

  /**
   * Callback invoked when the RPC has completed.
   */
  virtual void callback() = 0;


  bool invoke(RpcClient&       rClient,
              const RpcObject& rMarshalledCall,
              PbMessage*       pResponse,
              int              nTimeoutMs);

private:

  AsyncRpcSupervisor(const AsyncRpcSupervisor&);

  AsyncRpcSupervisor& operator= (const AsyncRpcSupervisor&);

  void run(const bool& bShutdown);

private:

  bool               cancelled_;
  RpcMarshalledCall* marshalled_call_;
  boost::mutex       call_lock_;
  PbMessage*         response_message_ptr_;
  int                timeout_ms_;
}; // End class AsyncRpcSupervisor

} // End namespace rpc
} // End namespace liber

#endif // ASYNC_RPC_SUPERVISOR_H
