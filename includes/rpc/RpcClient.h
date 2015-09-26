#ifndef RPC_CLIENT_H
#define RPC_CLIENT_H

#include <map>
#include <boost/thread/mutex.hpp>
#include <boost/uuid/uuid.hpp>
#include "Queue.h"
#include "RpcPacket.h"
#include "RpcMarshalledCall.h"
#include "PacketSubscriber.h"

namespace liber {
namespace rpc {

class RpcClient : public liber::netapp::PacketSubscriber {
public:

   RpcClient( liber::netapp::DestinationID server_destination_id );
   
   ~RpcClient();

   RpcMarshalledCall* invokeRpc(const RpcObject &object);

   bool put( liber::netapp::DestinationID destination_id, const void* data_ptr, ui32 length );

private:

   RpcMarshalledCall* getMarshalledRpc( const RpcObject &object );

   void processCancellations();

private:

   boost::mutex mRpcMutex;
   
   // Map each call to a unique identifier so that we can look up the
   // call when a response is received.
   typedef std::map<boost::uuids::uuid, RpcMarshalledCall*> CallMap;
   CallMap mRpcMap;

   liber::netapp::DestinationID server_destination_id_;
};

}}

#endif // RPC_CLIENT_H
