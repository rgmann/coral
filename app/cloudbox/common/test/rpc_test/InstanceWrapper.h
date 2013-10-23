#ifndef INSTANCE_WRAPPER_H
#define INSTANCE_WRAPPER_H

#include "RpcReturnValue.h"

class InstanceWrapper
{
public:

   typedef bool (*Method)(InstanceWrapper*, Structure&, RpcReturnValue&);
   
   InstanceWrapper(const Structure &params){};
   virtual ~InstanceWrapper(){};
};

#endif // INSTANCE_WRAPPER_H
