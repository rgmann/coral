#include <iostream>
#include <signal.h>
#include "ArgParser.h"
#include "BinarySem.h"
#include "RpcClient.h"
#include "TcpClient.h"
//#include "HeimdallControllerClientStub.h"
#include "TcpClientPacketRouter.h"
#include "RpcAppCommon.h"
#include "EtermCommands.h"
#include "InteractiveCommandRouter.h"

using namespace liber::rpc;
using namespace liber::netapp;
using namespace liber::net;
using namespace liber::cli;
//using namespace eterm;

void runClient(RpcClient& client)
{
  HeimdallControllerClientStub heimdallClient(client);
  eterm::LedCommand ledCommand(heimdallClient);
  eterm::ActivateDoorCommand activateDoorCommand(heimdallClient);
  eterm::EnrollUserCommand enrollUserCommand(heimdallClient);
  eterm::RemoveOneCommand removeOneCommand(heimdallClient);
  eterm::RemoveAllCommand removeAllCommand(heimdallClient);
  eterm::PrintUsersCommand printUsersCommand(heimdallClient);

  InteractiveCommandRouter commandRouter;
  commandRouter.add(&ledCommand);
  commandRouter.add(&activateDoorCommand);
  commandRouter.add(&enrollUserCommand);
  commandRouter.add(&removeOneCommand);
  commandRouter.add(&removeAllCommand);
  commandRouter.add(&printUsersCommand);

  commandRouter.run();
}

class DisconnectCallback : public liber::Callback {
public:

  DisconnectCallback(){};
  ~DisconnectCallback(){};

  void* call(void* pUserparam)
  {
    std::cout << "Disconnected from server." << std::endl;
    return NULL;
  }
};

int main(int argc, char *argv[])
{
  ArgParser args;
  args.addArg("name: HostName, primary: h, alt: host, type: opt, \
               vtype: string, required, desc: Host name/address");
  args.addArg("name: Port, primary: p, alt: port, type: opt, \
               vtype: int, required, desc: Set port number");

  if (!args.parse((const char**)argv, argc))
  {
    args.printArgErrors(true);
    return 1;
  }

  std::string lsHostName;
  args.getArgVal(lsHostName, Argument::ArgName, "HostName");

  int lnPort = 0;
  args.getArgVal(lnPort, Argument::ArgName, "Port");

  // To prevent the application from crashing, ignore SIGPIPE.
  signal(SIGPIPE, SIG_IGN);

  DisconnectCallback    callback;
  TcpClientPacketRouter router;
  RpcClient client;

  router.setDisconnectCallback(&callback);

  // Register the RPC client application.
  if (!router.addSubscriber(RPC_APP_ID, &client))
  {
    std::cout << "Failed to RPC as packet subscriber." << std::endl;
  }

  // Start the client packet router.
  if (router.start(lsHostName.c_str(), lnPort))
  {
    runClient(client);

    // Stop the client router.
    router.stop();
  }

  signal(SIGPIPE, SIG_DFL);

  return 0;
}


