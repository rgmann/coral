#ifndef DATABASE_WRAPPER_H
#define DATABASE_WRAPPER_H

#include "InstanceWrapper.h"
#include "rpc_getting_started.pb.h"
#include "Database.h"

class DatabaseWrapper : public liber::rpc::InstanceWrapper {
public:

   DatabaseWrapper();
   ~DatabaseWrapper();

   bool initialize(const std::string& params);
   bool destroy(const std::string& params);

   static void createDelegate(InstanceWrapper *pInst, const std::string& input, std::string& output, liber::rpc::RpcException&);
   static void readDelegate(InstanceWrapper *pInst, const std::string& input, std::string& output, liber::rpc::RpcException&);
   static void updateDelegate(InstanceWrapper *pInst, const std::string& input, std::string& output, liber::rpc::RpcException&);
   static void destroyDelegate(InstanceWrapper *pInst, const std::string& input, std::string& output, liber::rpc::RpcException&);


private:

   void create(const rpc_getting_started::UserAttributes& request, rpc_getting_started::DbStatus& response, liber::rpc::RpcException& e);
   void read(const rpc_getting_started::UserName& request, rpc_getting_started::UserAttributesList& response, liber::rpc::RpcException& e);
   void update(const rpc_getting_started::UserAttributes& request, rpc_getting_started::DbStatus& response, liber::rpc::RpcException& e);
   void destroy(const rpc_getting_started::UserName& request, rpc_getting_started::DbStatus& response, liber::rpc::RpcException& e);


private:

   // TODO: Add additional instance variables here.
   Database mDatabase;
};

#endif // DATABASE_WRAPPER_H
