#ifndef RPC_SUPERVISOR_H
#define RPC_SUPERVISOR_H

#include "RpcClient.h"

namespace liber {
namespace rpc {

class RpcSupervisor {
public:

  RpcSupervisor() {};
  virtual ~RpcSupervisor() {};

  virtual bool invoke(RpcClient&       rClient,
                      const RpcObject& rMarshalledCall,
                      PbMessage*       pResponse,
                      int              nTimeoutMs) = 0;

  RpcException& exception();
  RpcObject&    response();

private:
  
  RpcSupervisor(const RpcSupervisor&);

  RpcSupervisor& operator= (const RpcSupervisor&);

protected:

  RpcObject    response_object_;
  RpcException exception_;

}; // End class RpcSupervisor

} // End namespace rpc
} // End namespace liber

#endif // RPC_SUPERVISOR_H
