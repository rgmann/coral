#ifndef ASIO_TCP_CLIENT_H
#define ASIO_TCP_CLIENT_H

#include <boost/asio.hpp>
#include "AsioTcpPacketRouter.h"

namespace liber {
namespace netapp {

// class AsioTcpClient {
// public:

//    AsioTcpClient( boost::asio::io_service& io_service );

//    ~AsioTcpClient();

//    void connect( AsioTcpPacketRouterPtr router, const std::string& host, const std::string& port );

// private:

//    boost::asio::ip::tcp::resolver resolver_;
    
// };
class AsioTcpClientRouter : public AsioTcpPacketRouter {
public:

   AsioTcpClientRouter( boost::asio::io_service& io_service );

   ~AsioTcpClientRouter();

   static const int kWaitForever = -1;
   void connect( const std::string& host, const std::string& port, int timeout_secs = kWaitForever );


protected:

   virtual void afterConnect();


private:

   boost::asio::ip::tcp::resolver resolver_;
   boost::asio::deadline_timer    timer_;
    
};

typedef  boost::shared_ptr<AsioTcpClientRouter>  AsioTcpClientRouterPtr;

} // namespace netapp
} // namespace liber


#endif // ASIO_TCP_CLIENT_H