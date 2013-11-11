#ifndef DATABASE_UPDATE_INPUT_H
#define DATABASE_UPDATE_INPUT_H

#include "RpcParameterList.h"
#include "UserAttributes.h"

class DatabaseUpdateInput : public RpcParameterList {
public:

   DatabaseUpdateInput();
   DatabaseUpdateInput(const Structure& params);

   using RpcParameterList::marshall;
   void marshall(std::string lastname, const UserAttributes& attrs);

   using RpcParameterList::unmarshall;
   bool unmarshall(std::string& lastname, UserAttributes& attrs);

   bool isValid() const;

};

#endif // DATABASE_UPDATE_INPUT_H
