#ifndef DATABASE_CREATE_PARAM_LIST_H
#define DATABASE_CREATE_PARAM_LIST_H

#include "Structure.h"

class DatabasecreateParamListParamList : public Structure {
public:

   DatabasecreateParamListParamList();
   ~DatabasecreateParamListParamList();

   bool valid() const;

   bool get();
};

#endif // DATABASE_CREATE_PARAM_LIST_H
