#ifndef SEARCHSERVICE_WRAPPER_H
#define SEARCHSERVICE_WRAPPER_H

#include "RpcServerResource.h"
#include "tutorial.pb.h"

namespace tutorial {

class SearchServiceSearchAction : public liber::rpc::RpcServiceAction {
public:


   void operator() ( const std::string& request, std::string& response, liber::rpc::RpcException& e );

private:

   void Search( const Person& request, Person_PhoneNumber& response, liber::rpc::RpcException& e);
};

// class SearchServiceWrapper
// : public liber::rpc::InstanceWrapper {
// public:

//   explicit SearchServiceWrapper();
//   ~SearchServiceWrapper();

//   // bool initialize(const std::string& params);
//   // bool destroy(const std::string& params);

//   static void DELEGATE(SearchDelegate);

// private:

//   void ACTION(Search, Person, Person_PhoneNumber);

// private:

//   // TODO: Add additional instance variables here.
//    static SearchService service_;

// }; // End SearchServiceWrapper

}  // End namespace tutorial


#endif // SEARCHSERVICE_CLIENT_STUB_H
