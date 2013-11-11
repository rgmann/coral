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

   static RpcException createDelegate(InstanceWrapper *pInst, const Structure& input, Structure& output);
   static RpcException readDelegate(InstanceWrapper *pInst, const Structure& input, Structure& output);
   static RpcException updateDelegate(InstanceWrapper *pInst, const Structure& input, Structure& output);
   static RpcException destroyDelegate(InstanceWrapper *pInst, const Structure& input, Structure& output);

private:

   bool create(std::string lastname, const UserAttributes& attrs);
   bool read(std::string lastname, UserAttributes& attrs);
   bool update(std::string lastname, const UserAttributes& attrs);
   bool destroy(std::string lastname);

private:

   Database* mpInst;
};

#endif // DATABASE_WRAPPER_H
