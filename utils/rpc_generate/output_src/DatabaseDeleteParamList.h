#ifndef DATABASE_DELETE_PARAM_LIST_H
#define DATABASE_DELETE_PARAM_LIST_H

#include "Structure.h"

class DatabaseDeleteParamList : public Structure {
public:

   DatabaseDeleteParamList();
   ~DatabaseDeleteParamList();

   bool valid() const;

   bool get();
};

#endif // DATABASE_DELETE_PARAM_LIST_H
