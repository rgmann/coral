#ifndef DATABASE_READ_PARAM_LIST_H
#define DATABASE_READ_PARAM_LIST_H

#include "Structure.h"

class DatabasereadParamListParamList : public Structure {
public:

   DatabasereadParamListParamList();
   ~DatabasereadParamListParamList();

   bool valid() const;

   bool get();
};

#endif // DATABASE_READ_PARAM_LIST_H
