#ifndef SEARCHSERVICE_CLIENT_STUB_H
#define SEARCHSERVICE_CLIENT_STUB_H

#include "RpcClientResource.h"
#include "tutorial.pb.h"

namespace tutorial {

class SearchServiceClientStub
: public coral::rpc::RpcClientResource {
public:

  explicit SearchServiceClientStub(coral::rpc::RpcClient &client);
  ~SearchServiceClientStub();

  void Search(const Person& request, Person_PhoneNumber& response, coral::rpc::AsyncRpcSupervisor* pSupervisor = NULL)
         throw (coral::rpc::RpcException);

}; // End SearchServiceClientStub

}  // End namespace tutorial


#endif // SEARCHSERVICE_CLIENT_STUB_H
