#ifndef HEIMDALL_CONTROLLER_H
#define HEIMDALL_CONTROLLER_H

#include <boost/array.hpp>
#include <boost/asio.hpp>

#include "BaseTypes.h"
#include "Mutex.h"
#include "IThread.h"
#include "Queue.h"
#include "RpcAppCommon.h"
#include "UserDb.h"


namespace eterm {

enum HeimdallCommands {
  HC_CMD_ACK = 1,
  HC_SET_LED_STATE,
  HC_ACTIVATE_DOOR,
  HC_ENROLL_NEW,
  HC_GET_NEW_ENROLLED,
  HC_GET_NEW_IDD,
  HC_REMOVE_ONE,
  HC_REMOVE_ALL
};

struct HcLedCommand {
  ui8 ledID;
  bool ledOn;
};

struct HcNewEnrolled {
  ui8 userID;
};

struct HcCommandAck {
  bool success;
};

typedef HcNewEnrolled HcNewIdentified;
typedef HcNewEnrolled HcRemoveOne;

struct __attribute__((__packed__)) HcMessage {
  struct PacketHeader {
    ui32 marker;
    i16  type;
    i16  length;
  } header;

  union {
    HcCommandAck    ack;
    HcLedCommand    ledState;
    HcNewEnrolled   newEnrolled;
    HcNewIdentified newIdentified;
    HcRemoveOne     removeOne;
  } data;
};

class HeimdallController : liber::concurrency::IThread {
public:

  HeimdallController(const char* pHost, int nPort);
  ~HeimdallController();

  struct Status {
    bool connected;
    bool busy;
  };

  Status setLedState(LedId id, bool bOn);
  Status activateDoor();
  Status enroll(const std::string& first, const std::string& last);

  Status removeOne(ui8 id);
  Status removeAll();

  inline UserDb& db() { return mUserDb; };

private:

  void run(const bool& bShutdown);

  bool connect(boost::asio::ip::tcp::socket& rSocket, boost::asio::io_service& rService);

  bool sendCommand(boost::asio::ip::tcp::socket& rSocket, const HcMessage& rMessage); 
  bool recvResponse(boost::asio::ip::tcp::socket& rSocket, HcMessage& rMessage);

private:

  const char* mpHost;
  int mnPort;

  UserDb mUserDb;

  Mutex mActiveEnrollMutex;
  rpc_eterm::User* mpActiveUser;
//  bool  mbBusyEnrolling;

  Queue<HcMessage> mCommands;

//  bool mbConnected;
  Status mStatus;
};

}

#endif // HEIMDALL_CONTROLLER_H
