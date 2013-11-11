#ifndef DATABASE_UPDATE_OUTPUT_H
#define DATABASE_UPDATE_OUTPUT_H

#include "RpcParameterList.h"

class DatabaseUpdateOutput : public RpcParameterList {
public:

   DatabaseUpdateOutput();
   DatabaseUpdateOutput(const Structure& params);

   using RpcParameterList::marshall;

   using RpcParameterList::unmarshall;

   bool isValid() const;

   void setReturnValue(bool returnValue);
   bool getReturnValue(bool& returnValue) const;
};

#endif // DATABASE_UPDATE_OUTPUT_H
