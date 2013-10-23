#ifndef DATABASE_UPDATE_PARAM_LIST_H
#define DATABASE_UPDATE_PARAM_LIST_H

#include "Structure.h"

class DatabaseUpdateParamList : public Structure {
public:

   DatabaseUpdateParamList();
   ~DatabaseUpdateParamList();

   bool valid() const;

   bool get();
};

#endif // DATABASE_UPDATE_PARAM_LIST_H
