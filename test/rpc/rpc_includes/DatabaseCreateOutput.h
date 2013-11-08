#ifndef DATABASE_CREATE_OUTPUT_H
#define DATABASE_CREATE_OUTPUT_H

#include "RpcParameterList.h"

class DatabaseCreateOutput : public RpcParameterList {
public:

   DatabaseCreateOutput();
   DatabaseCreateOutput(const Structure& params);

   using RpcParameterList::marshall;

   using RpcParameterList::unmarshall;

   bool isValid() const;

   void setReturnValue(bool returnValue);
   bool getReturnValue(bool& returnValue) const;
};

#endif // DATABASE_CREATE_OUTPUT_H
