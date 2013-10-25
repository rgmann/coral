#ifndef DATABASEREAD_PARAM_LIST_H
#define DATABASEREAD_PARAM_LIST_H

#include "Structure.h"

class DatabasereadParamList : public Structure {
public:

   DatabasereadParamList();
   ~DatabasereadParamList();

   bool valid() const;

   bool get();
};

#endif // DATABASEREAD_PARAM_LIST_H
