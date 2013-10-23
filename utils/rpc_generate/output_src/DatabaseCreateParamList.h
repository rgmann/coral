#ifndef DATABASE_CREATE_PARAM_LIST_H
#define DATABASE_CREATE_PARAM_LIST_H

#include "Structure.h"

class DatabaseCreateParamList : public Structure {
public:

   DatabaseCreateParamList();
   ~DatabaseCreateParamList();

   bool valid() const;

   bool get();
};

#endif // DATABASE_CREATE_PARAM_LIST_H
