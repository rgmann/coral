// %% license-end-token %%

// WARNING: Auto-generated code. Do not modify.

#ifndef SEARCHSERVICE_SERVER_STUB_H
#define SEARCHSERVICE_SERVER_STUB_H

#include "RpcServerResource.h"

#include "SearchServiceSearchAction.h"
#include "SearchServiceAddAction.h"

namespace tutorial {

class SearchServiceServerStub : public liber::rpc::RpcServerResource {
public:

   explicit SearchServiceServerStub();
   ~SearchServiceServerStub();

   virtual void registerActions();


private:

   SearchServiceSearchAction default_search_action_;
   SearchServiceAddAction default_add_action_;

}; // End SearchServiceServerStub

}  // End namespace tutorial


#endif // SEARCHSERVICE_SERVER_STUB_H
