#include <iostream>
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
  eterm::FpStatusCommand fpCommand(heimdallClient);

  InteractiveCommandRouter commandRouter;
  commandRouter.add(&ledCommand);
  commandRouter.add(&fpCommand);

  commandRouter.run();
}

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

  TcpClientPacketRouter router;
  RpcClient client;

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
 
  return 0;
}


