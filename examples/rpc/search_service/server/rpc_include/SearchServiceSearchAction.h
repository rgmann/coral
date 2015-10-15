#ifndef SEARCHSERVICE_SEARCH_ACTION_H
#define SEARCHSERVICE_SEARCH_ACTION_H
#include "RpcServerResource.h"
#include "tutorial.pb.h"

namespace tutorial {

class SearchServiceSearchAction : public coral::rpc::RpcServiceAction {
public:

   SearchServiceSearchAction();

   void operator() (
      const std::string&        request,
      std::string&              response,
      coral::rpc::RpcException& e );

protected:

   virtual void Search(
      const Person& request,
      Person_PhoneNumber& response,
      coral::rpc::RpcException& e );
};

}  // End namespace tutorial

#endif // SEARCHSERVICE_SEARCH_ACTION_H
