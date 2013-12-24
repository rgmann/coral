#ifndef RPC_OBJECT_H
#define RPC_OBJECT_H

#include <string>
#include <google/protobuf/message.h>
#include "RpcException.h"

namespace liber {
namespace rpc {

typedef ::google::protobuf::Message PbMessage;


class RpcObject {
public:

   RpcObject();
   RpcObject(const std::string &resourceName, const std::string &actionName);
   virtual ~RpcObject();

   //--------------------------------------------------------------------------
   //                            Public Fields
   //--------------------------------------------------------------------------
   RpcCallInfo&  callInfo();
   const RpcCallInfo&  callInfo() const;
   RpcException& exception();
   const RpcException& exception() const;


   //--------------------------------------------------------------------------
   //                            Public Methods
   //--------------------------------------------------------------------------
   void setParams(const PbMessage& message);
   void setParams(const std::string& message);
   void getParams(PbMessage& message) const;
   void getParams(std::string& message) const;
   
   virtual bool isValid() const;
      
   
   bool getResponse(RpcObject &response) const;
   bool getResponse(RpcObject &response, const PbMessage &value) const;
   bool getResponse(RpcObject &response, const std::string &value) const;

   std::string serialize() const;
   bool deserialize(const std::string& data);


   std::string  mMessage;
private:

   // Protobuf message serialized to a byte string.
   //std::string  mMessage;
   RpcCallInfo  mCallInfo;
   RpcException mException;
};

}}

#endif // RPC_OBJECT_H
