// 
// Copyright (c) 2015, Robert Glissmann
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// 
// * Redistributions of source code must retain the above copyright notice, this
// list of conditions and the following disclaimer.
// 
// * Redistributions in binary form must reproduce the above copyright notice,
// this list of conditions and the following disclaimer in the documentation
// and/or other materials provided with the distribution.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// 

// %% license-end-token %%
// 
// Author: Robert.Glissmann@gmail.com (Robert Glissmann)
// 
// 



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
