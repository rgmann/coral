#ifndef DATABASE_DELETE_PARAM_LIST_H
#define DATABASE_DELETE_PARAM_LIST_H

#include "Structure.h"

class DatabasedeleteParamListParamList : public Structure {
public:

   DatabasedeleteParamListParamList();
   ~DatabasedeleteParamListParamList();

   bool valid() const;

   bool get();
};

#endif // DATABASE_DELETE_PARAM_LIST_H
