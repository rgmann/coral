#ifndef SEARCHSERVICE_ADD_ACTION_H
#define SEARCHSERVICE_ADD_ACTION_H
#include "RpcServerResource.h"
#include "tutorial.pb.h"

namespace tutorial {

class SearchServiceAddAction : public liber::rpc::RpcServiceAction {
public:

   SearchServiceServiceAddAction();

   void operator() (
      const std::string&        request,
      std::string&              response,
      liber::rpc::RpcException& e );

protected:

   virtual void Add(
      const Person& request,
      Person_PhoneNumber& response,
      liber::rpc::RpcException& e );
};

}  // End namespace tutorial

#endif // SEARCHSERVICE_ADD_ACTION_H
