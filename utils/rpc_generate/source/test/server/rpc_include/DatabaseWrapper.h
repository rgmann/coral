#ifndef DATABASE_WRAPPER_H
#define DATABASE_WRAPPER_H

#include "InstanceWrapper.h"
#include "rpc_calculator_app.pb.h"

class DatabaseWrapper : public liber::rpc::InstanceWrapper {
public:

   DatabaseWrapper();
   ~DatabaseWrapper();

   bool initialize(const std::string& params);
   bool destroy(const std::string& params);

   static void createDelegate(InstanceWrapper *pInst, const std::string& input, std::string& output, RpcException&);
   static void readDelegate(InstanceWrapper *pInst, const std::string& input, std::string& output, RpcException&);
   static void updateDelegate(InstanceWrapper *pInst, const std::string& input, std::string& output, RpcException&);
   static void destroyDelegate(InstanceWrapper *pInst, const std::string& input, std::string& output, RpcException&);


private:

   liber::rpc::RpcException create(const rpc_calculator_app::UserAttributes& request, rpc_calculator_app::DbStatus& response, liber::rpc::RpcException& e);
   liber::rpc::RpcException read(const rpc_calculator_app::UserName& request, rpc_calculator_app::UserAttributesList& response, liber::rpc::RpcException& e);
   liber::rpc::RpcException update(const rpc_calculator_app::UserAttributes& request, rpc_calculator_app::DbStatus& response, liber::rpc::RpcException& e);
   liber::rpc::RpcException destroy(const rpc_calculator_app::UserName& request, rpc_calculator_app::DbStatus& response, liber::rpc::RpcException& e);


private:

   // TODO: Add additional instance variables here.
};

#endif // DATABASE_WRAPPER_H
