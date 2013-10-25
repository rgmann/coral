#ifndef DATABASEUPDATE_PARAM_LIST_H
#define DATABASEUPDATE_PARAM_LIST_H

#include "Structure.h"

class DatabaseupdateParamList : public Structure {
public:

   DatabaseupdateParamList();
   ~DatabaseupdateParamList();

   bool valid() const;

   bool get();
};

#endif // DATABASEUPDATE_PARAM_LIST_H
