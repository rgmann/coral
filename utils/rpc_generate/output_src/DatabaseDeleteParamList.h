#ifndef DATABASEDELETE_PARAM_LIST_H
#define DATABASEDELETE_PARAM_LIST_H

#include "Structure.h"

class DatabasedeleteParamList : public Structure {
public:

   DatabasedeleteParamList();
   ~DatabasedeleteParamList();

   bool valid() const;

   bool get();
};

#endif // DATABASEDELETE_PARAM_LIST_H
