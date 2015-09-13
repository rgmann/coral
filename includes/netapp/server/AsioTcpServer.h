#ifndef  ASIO_TCP_SERVER_H
#define  ASIO_TCP_SERVER_H

#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include "AsioTcpPacketRouter.h"

namespace liber {
namespace netapp {

class AsioTcpServer {
public:

   AsioTcpServer(
      boost::asio::io_service&              io_service,
      const boost::asio::ip::tcp::endpoint& endpoint );
   virtual ~AsioTcpServer();

   void startAccept();


protected:

   virtual AsioTcpPacketRouterPtr createRouter( boost::asio::io_service& io_service ) = 0;

private:

   void handleAccept( AsioTcpPacketRouterPtr session, const boost::system::error_code& error );

private:

   boost::asio::io_service& io_service_;

   boost::asio::ip::tcp::acceptor acceptor_;

};

typedef boost::shared_ptr<AsioTcpServer> AsioTcpServerPtr;


} // namespace netapp
} // namespace liber

#endif // ASIO_TCP_SERVER_H