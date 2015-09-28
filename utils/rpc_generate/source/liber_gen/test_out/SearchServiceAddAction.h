#ifndef SEARCHSERVICE_SERVICE_ADD_ACTION_H
#define SEARCHSERVICE_SERVICE_ADD_ACTION_H
#include "RpcServerResource.h"
#include "tutorial.pb.h"

namespace tutorial {

class SearchServiceServiceAddAction : public liber::rpc::RpcServiceAction {
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

#endif // SEARCHSERVICE_SERVICE_ADD_ACTION_H
