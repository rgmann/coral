// 
// Copyright (c) 2015, Robert Glissmann
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// 
// * Redistributions of source code must retain the above copyright notice, this
// list of conditions and the following disclaimer.
// 
// * Redistributions in binary form must reproduce the above copyright notice,
// this list of conditions and the following disclaimer in the documentation
// and/or other materials provided with the distribution.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// 

// %% license-end-token %%
// 
// Author: Robert.Glissmann@gmail.com (Robert Glissmann)
// 
// 


#ifndef RPC_SERVER_RESOURCE_H
#define RPC_SERVER_RESOURCE_H

#include <map>
#include <string>
#include <boost/uuid/uuid.hpp>
#include "Queue.h"
#include <boost/thread/mutex.hpp>
#include "RpcObject.h"
#include "Md5Hash.h"

namespace coral {
namespace rpc {


///
/// Abstract base class for all service actions (i.e. methods). Actions
/// receive a request, process the request, and then reply with a response.
///
/// This interface is implemented by actions generated by the
/// protoc-gen-coralrpc plug-in. A derived action is generated for each 'rpc'
/// definition within a 'service' definition.
///
class RpcServiceAction {
public:

   ///
   /// Construct action with specified name. This name should be unique among
   /// all other actions belong to the same service.
   ///
   /// @param  action_name  Service action name
   ///
   RpcServiceAction( const std::string& action_name )
      : action_name_( action_name ){}

   ///
   /// Get the action name
   ///
   /// @return string  Action name
   ///
   const std::string& getName() const { return action_name_; }

   ///
   /// Invoke the action
   ///
   /// @param  request  Action input arguments
   /// @param  response Action result
   /// @param  e        Exception status
   /// @return void
   ///
   virtual void operator() ( const std::string& request,
                             std::string&       response,
                             RpcException&      e ) = 0;

private:

   std::string action_name_;
};

typedef std::map<std::string,RpcServiceAction*>  ActionTable;


///
/// Abstract base class for all service/resource implementations.
/// An RpcServiceResource is a collection of related actions. A derived
/// RpcServerResource is generated for each 'service' definition in
/// a .proto file.
///
class RpcServerResource {
public:
   
   ///
   /// Construct a server service with the specified name. Two different
   /// services may not have the same name.
   ///
   /// @param  name  Service name
   ///
   RpcServerResource( const std::string& name );

   ///
   /// Unmarshall and route an action request
   ///
   /// @param  request  Request object
   /// @param  response Response object
   /// @return bool  True if packet was request was successfully unmarshalled
   ///               and routed to a valid action; false otherwise
   ///
   bool unmarshall( RpcObject& input, RpcObject& output );
   
   ///
   /// Get the service name
   ///
   /// @return  string  Resource name
   ///
   const std::string& getName() const;

   ///
   /// Register an action with the service. If an action with the same name
   /// already exists, it is removed.
   ///
   /// @param  action_ptr  Pointer to action instance
   /// @return bool        True on success; false on failure
   ///
   bool addAction( RpcServiceAction* action_ptr );

   ///
   /// Register all default actions associated with this service. This method
   /// should be overridden by derived classes.
   ///
   /// @return void
   ///
   virtual void registerActions() = 0;


protected:

   ///
   /// Invoke the requested action.
   ///
   /// @param  request  Request object
   /// @param  response Response object
   ///
   bool invoke( RpcObject& input, RpcObject& output);

   ///
   /// Build an exception response with the specified error ID and message.
   ///
   /// @param  request  Request object
   /// @param  response Response object
   /// @return void
   ///
   void exception( RpcObject&         request,
                   RpcObject&         response,
                   RpcErrorId         eid,
                   const std::string& message = "" );


private:

   ///
   /// Copies are not permitted.
   ///
   RpcServerResource( const RpcServerResource& );
   RpcServerResource& operator= ( const RpcServerResource& );


private:
   
   std::string resource_name_;

   ActionTable actions_;
   
};

} // end namespace rpc
} // end namespace coral

#endif // RPC_SERVER_RESOURCE_H
