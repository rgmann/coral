#ifndef DATABASE_DESTROY_OUTPUT_H
#define DATABASE_DESTROY_OUTPUT_H

#include "RpcParameterList.h"

class DatabaseDestroyOutput : public RpcParameterList {
public:

   DatabaseDestroyOutput();
   DatabaseDestroyOutput(const Structure& params);

   using RpcParameterList::marshall;

   using RpcParameterList::unmarshall;

   bool isValid() const;

   void setReturnValue(bool returnValue);
   bool getReturnValue(bool& returnValue) const;
};

#endif // DATABASE_DESTROY_OUTPUT_H
