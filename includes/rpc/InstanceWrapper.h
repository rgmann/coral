#ifndef INSTANCE_WRAPPER_H
#define INSTANCE_WRAPPER_H

#include "Structure.h"
#include "RpcCommon.h"

class InstanceWrapper
{
public:

   typedef RpcException (*Method)(InstanceWrapper*, const Structure&, Structure&);
   
   InstanceWrapper(){};
   virtual ~InstanceWrapper(){};
   
   virtual bool initialize(const Structure &params) = 0;
   virtual bool destroy(const Structure &params) = 0;
};

#endif // INSTANCE_WRAPPER_H
