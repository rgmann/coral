#ifndef BLOCKING_RPC_SUPERVISOR_H
#define BLOCKING_RPC_SUPERVISOR_H

#include "RpcSupervisor.h"

namespace liber {
namespace rpc {

class BlockingRpcSupervisor : public RpcSupervisor {
public:

  BlockingRpcSupervisor();
  ~BlockingRpcSupervisor();

  bool invoke(RpcClient&       rClient,
              const RpcObject& rMarshalledCall,
              PbMessage*       pResponse,
              int              nTimeoutMs);


private:

  BlockingRpcSupervisor(const BlockingRpcSupervisor&);

  BlockingRpcSupervisor& operator= (const BlockingRpcSupervisor&);

}; // End class BlockingRpcSupervisor

} // End namespace rpc
} // End namespace liber

#endif // BLOCKING_RPC_SUPERVISOR_H
