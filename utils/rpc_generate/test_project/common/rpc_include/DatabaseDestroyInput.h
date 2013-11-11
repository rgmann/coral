#ifndef DATABASE_DESTROY_INPUT_H
#define DATABASE_DESTROY_INPUT_H

#include "RpcParameterList.h"

class DatabaseDestroyInput : public RpcParameterList {
public:

   DatabaseDestroyInput();
   DatabaseDestroyInput(const Structure& params);

   using RpcParameterList::marshall;
   void marshall(std::string lastname);

   using RpcParameterList::unmarshall;
   bool unmarshall(std::string& lastname);

   bool isValid() const;

};

#endif // DATABASE_DESTROY_INPUT_H
