#ifndef SEARCHSERVICE_CLIENT_STUB_H
#define SEARCHSERVICE_CLIENT_STUB_H

#include "RpcClientResource.h"
#include "tutorial.pb.h"

namespace tutorial {

class SearchServiceClientStub
: public liber::rpc::RpcClientResource {
public:

  explicit SearchServiceClientStub(liber::rpc::RpcClient &client);
  ~SearchServiceClientStub();

  void Search(const Person& request, Person_PhoneNumber& response, liber::rpc::AsyncRpcSupervisor* pSupervisor = NULL)
         throw (liber::rpc::RpcException);
  void Add(const Person& request, Person_PhoneNumber& response, liber::rpc::AsyncRpcSupervisor* pSupervisor = NULL)
         throw (liber::rpc::RpcException);

}; // End SearchServiceClientStub

}  // End namespace tutorial


#endif // SEARCHSERVICE_CLIENT_STUB_H