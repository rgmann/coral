// %% license-end-token %%

// WARNING: Auto-generated code. Do not modify.
#ifndef SEARCHSERVICE_SEARCH_ACTION_H
#define SEARCHSERVICE_SEARCH_ACTION_H

#include "RpcServerResource.h"
#include "tutorial.pb.h"

namespace tutorial {

class SearchServiceSearchAction : public liber::rpc::RpcServiceAction {
public:

   SearchServiceSearchAction();

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
