#ifndef DATABASE_CREATE_INPUT_H
#define DATABASE_CREATE_INPUT_H

#include "RpcParameterList.h"
#include "UserAttributes.h"

class DatabaseCreateInput : public RpcParameterList {
public:

   DatabaseCreateInput();
   DatabaseCreateInput(const Structure& params);

   using RpcParameterList::marshall;
   void marshall(std::string lastname, const UserAttributes& attrs);

   using RpcParameterList::unmarshall;
   bool unmarshall(std::string& lastname, UserAttributes& attrs);

   bool isValid() const;

};

#endif // DATABASE_CREATE_INPUT_H
