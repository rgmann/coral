#ifndef DATABASE_READ_OUTPUT_H
#define DATABASE_READ_OUTPUT_H

#include "RpcParameterList.h"
#include "UserAttributes.h"

class DatabaseReadOutput : public RpcParameterList {
public:

   DatabaseReadOutput();
   DatabaseReadOutput(const Structure& params);

   using RpcParameterList::marshall;
   void marshall(UserAttributes& attrs);

   using RpcParameterList::unmarshall;
   bool unmarshall(UserAttributes& attrs);

   bool isValid() const;

   void setReturnValue(bool returnValue);
   bool getReturnValue(bool& returnValue) const;
};

#endif // DATABASE_READ_OUTPUT_H
