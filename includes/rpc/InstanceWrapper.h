#ifndef INSTANCE_WRAPPER_H
#define INSTANCE_WRAPPER_H

#include "RpcReturnValue.h"

class InstanceWrapper
{
public:

   typedef void (*Method)(InstanceWrapper*, Structure&, RpcReturnValue&);
   
   InstanceWrapper(){};
   virtual ~InstanceWrapper(){};
   
   virtual bool initialize(const Structure &params) = 0;
   virtual bool destroy(const Structure &params) = 0;
};

#endif // INSTANCE_WRAPPER_H
