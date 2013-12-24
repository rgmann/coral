#ifndef RPC_PARAMETER_LIST_H
#define RPC_PARAMETER_LIST_H

#include "Structure.h"
#include "RpcCommon.h"

class RpcParameterList
{
public:

   RpcParameterList() {};
   RpcParameterList(const Structure &params)
   {
      myParameters = params;
   };
   virtual ~RpcParameterList(){};

   virtual void marshall() {};

   virtual bool unmarshall() { return true; };

   virtual bool isValid() const = 0;

   Structure& getParameters()
   {
      return myParameters;
   };

protected:

  Structure myParameters;
};

#endif // RPC_PARAMETER_LIST_H
