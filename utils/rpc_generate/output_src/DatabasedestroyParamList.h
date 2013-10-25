#ifndef DATABASEDESTROY_PARAM_LIST_H
#define DATABASEDESTROY_PARAM_LIST_H

#include "Structure.h"

class DatabasedestroyParamList : public Structure {
public:

   DatabasedestroyParamList();
   ~DatabasedestroyParamList();

   bool valid() const;

   bool get();
};

#endif // DATABASEDESTROY_PARAM_LIST_H
