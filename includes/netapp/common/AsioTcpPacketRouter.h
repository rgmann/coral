#ifndef  ASIO_TCP_PACKET_ROUTER_H
#define  ASIO_TCP_PACKET_ROUTER_H

namespace liber {
namespace netapp {

class AsioTcpPacketRouter : public liber::netapp::PacketRouter {
public:

  AsioTcpPacketRouter(
    boost::asio::io_service&  io_service,
    tcp::resolver::iterator   endpoint_iterator );

  virtual ~AsioTcpPacketRouter();


private:

  

private:

  boost::asio::io_service& io_service_;
  tcp::socket socket_;
  NetAppPacket::Data read_packet_header_;
  NetAppPacket                read_packet_;
  std::deque<NetAppPacket*>   write_packets_;
  PacketQueue                 packet_receiver;
};

}
}


#endif // ASIO_TCP_PACKET_ROUTER_H