#ifndef DATABASE_WRAPPER_H
#define DATABASE_WRAPPER_H

#include "InstanceWrapper.h"
#include "Database.h"

class DatabaseWrapper : public InstanceWrapper {
public:

   DatabaseWrapper();
   ~DatabaseWrapper();

   bool initialize(const Structure& params);
   bool destroy(const Structure& params);

   static void create(InstanceWrapper *pInst, Structure &params, RpcReturnValue &retobj);
   static void read(InstanceWrapper *pInst, Structure &params, RpcReturnValue &retobj);
   static void update(InstanceWrapper *pInst, Structure &params, RpcReturnValue &retobj);
   static void delete(InstanceWrapper *pInst, Structure &params, RpcReturnValue &retobj);

private:

   Database* mpInst;
};
#endif // DATABASE_WRAPPER_H
