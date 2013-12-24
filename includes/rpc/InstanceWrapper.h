#ifndef INSTANCE_WRAPPER_H
#define INSTANCE_WRAPPER_H

#include "RpcCommon.h"
#include "RpcException.h"

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
