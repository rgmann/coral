#ifndef DATABASE_UPDATE_PARAM_LIST_H
#define DATABASE_UPDATE_PARAM_LIST_H

#include "Structure.h"

class DatabaseupdateParamListParamList : public Structure {
public:

   DatabaseupdateParamListParamList();
   ~DatabaseupdateParamListParamList();

   bool valid() const;

   bool get();
};

#endif // DATABASE_UPDATE_PARAM_LIST_H
