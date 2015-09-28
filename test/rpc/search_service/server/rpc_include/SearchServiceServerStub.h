#ifndef SEARCHSERVICE_SERVER_STUB_H
#define SEARCHSERVICE_SERVER_STUB_H

#include "RpcServerResource.h"
#include "SearchServiceWrapper.h"

namespace tutorial {

class SearchServiceServerStub
: public liber::rpc::RpcServerResource {
public:

  explicit SearchServiceServerStub();
  ~SearchServiceServerStub();

  virtual void registerActions();

private:

  SearchServiceSearchAction default_search_action_;

}; // End SearchServiceServerStub

}  // End namespace tutorial


#endif // SEARCHSERVICE_SERVER_STUB_H
