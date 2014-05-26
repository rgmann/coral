#ifndef SEARCHSERVICE_WRAPPER_H
#define SEARCHSERVICE_WRAPPER_H

#include "InstanceWrapper.h"
#include "tutorial.pb.h"

namespace tutorial {

class SearchServiceWrapper
: public liber::rpc::InstanceWrapper {
public:

  explicit SearchServiceWrapper();
  ~SearchServiceWrapper();

  bool initialize(const std::string& params);
  bool destroy(const std::string& params);

  static void DELEGATE(SearchDelegate);

private:

  void ACTION(Search, Person, Person_PhoneNumber);

private:

  // TODO: Add additional instance variables here.

}; // End SearchServiceWrapper

}  // End namespace tutorial


#endif // SEARCHSERVICE_CLIENT_STUB_H
