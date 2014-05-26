#ifndef INSTANCE_WRAPPER_H
#define INSTANCE_WRAPPER_H

#include "RpcCommon.h"
#include "RpcException.h"

#define DELEGATE(action_delegate)                        \
  action_delegate(InstanceWrapper *pInst,                \
              const std::string& request,                \
              std::string& response,                     \
              liber::rpc::RpcException& e)

#define ACTION(action_name, request_type, response_type) \
  action_name(const request_type& request,               \
              response_type& response,                   \
              liber::rpc::RpcException& e)


namespace liber {
namespace rpc {

class InstanceWrapper {
public:

   typedef void (*Method)(InstanceWrapper*, const std::string&, std::string&, RpcException&);
   
   InstanceWrapper(){};
   virtual ~InstanceWrapper(){};
   
   virtual bool initialize(const std::string &params) = 0;
   virtual bool destroy(const std::string &params) = 0;
};

}}

#endif // INSTANCE_WRAPPER_H
