#ifndef DATABASE_READ_INPUT_H
#define DATABASE_READ_INPUT_H

#include "RpcParameterList.h"
#include "UserAttributes.h"

class DatabaseReadInput : public RpcParameterList {
public:

   DatabaseReadInput();
   DatabaseReadInput(const Structure& params);

   using RpcParameterList::marshall;
   void marshall(std::string lastname, UserAttributes& attrs);

   using RpcParameterList::unmarshall;
   bool unmarshall(std::string& lastname, UserAttributes& attrs);

   bool isValid() const;

};

#endif // DATABASE_READ_INPUT_H
