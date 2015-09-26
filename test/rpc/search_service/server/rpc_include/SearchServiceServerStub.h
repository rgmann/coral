#ifndef SEARCHSERVICE_SERVER_STUB_H
#define SEARCHSERVICE_SERVER_STUB_H

#include "RpcServerResource.h"
#include "SearchServiceWrapper.h"

namespace tutorial {

struct SearchServiceHookCb;

class SearchServiceServerStub
: public liber::rpc::RpcServerResource {
public:

  explicit SearchServiceServerStub();
  ~SearchServiceServerStub();

  virtual void registerActions();


protected:


private:


}; // End SearchServiceServerStub

}  // End namespace tutorial


#endif // SEARCHSERVICE_SERVER_STUB_H
