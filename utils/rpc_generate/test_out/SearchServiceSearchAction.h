#ifndef SEARCHSERVICE_SEARCH_ACTION_H
#define SEARCHSERVICE_SEARCH_ACTION_H
#include "RpcServerResource.h"
#include "tutorial.pb.h"

namespace tutorial {

class SearchServiceSearchAction : public liber::rpc::RpcServiceAction {
public:

   SearchServiceServiceSearchAction();

   void operator() (
      const std::string&        request,
      std::string&              response,
      liber::rpc::RpcException& e );

protected:

   virtual void Search(
      const Person& request,
      Person_PhoneNumber& response,
      liber::rpc::RpcException& e );
};

}  // End namespace tutorial

#endif // SEARCHSERVICE_SEARCH_ACTION_H
