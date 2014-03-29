#ifndef RSYNCSERVICE_WRAPPER_H
#define RSYNCSERVICE_WRAPPER_H

#include "InstanceWrapper.h"
#include "rpc_rsync_service.pb.h"

class RsyncServiceWrapper : public liber::rpc::InstanceWrapper {
public:

   RsyncServiceWrapper();
   ~RsyncServiceWrapper();

   liber::rsync::RsyncPacketSubscriber* getRsyncPacketSubscriber();

   bool initialize(const std::string& params);
   bool destroy(const std::string& params);

   static void queryFileDelegate(InstanceWrapper *pInst, const std::string& input, std::string& output, liber::rpc::RpcException&);
   static void pullDelegate(InstanceWrapper *pInst, const std::string& input, std::string& output, liber::rpc::RpcException&);
   static void pushDelegate(InstanceWrapper *pInst, const std::string& input, std::string& output, liber::rpc::RpcException&);


private:

   void queryFile(const rpc_rsync_service::FileQuery& request, rpc_rsync_service::FileQueryResponse& response, liber::rpc::RpcException& e);
   void pull(const rpc_rsync_service::JobStart& request, rpc_rsync_service::JobStartResponse& response, liber::rpc::RpcException& e);
   void push(const rpc_rsync_service::JobStart& request, rpc_rsync_service::JobStartResponse& response, liber::rpc::RpcException& e);


private:

   // TODO: Add additional instance variables here.
   liber::rsync::RsyncPacketSubscriber mRsyncSubscriber;
   liber::rsync::JobManager mJobManager;
};

#endif // RSYNCSERVICE_WRAPPER_H
