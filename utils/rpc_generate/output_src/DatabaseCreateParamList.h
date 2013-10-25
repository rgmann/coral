#ifndef DATABASECREATE_PARAM_LIST_H
#define DATABASECREATE_PARAM_LIST_H

#include "Structure.h"

class DatabasecreateParamList : public Structure {
public:

   DatabasecreateParamList();
   ~DatabasecreateParamList();

   bool valid() const;

   bool get();
};

#endif // DATABASECREATE_PARAM_LIST_H
