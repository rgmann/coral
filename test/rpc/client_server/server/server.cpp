#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
//#include "ArgParser.h"
#include "RpcServerListener.h"

bool  g_bStopCommanded = false;

void sighandler(int s)
{
   g_bStopCommanded = true;
}

int main(int argc, char *argv[])
{
   //ArgParser args;
   RpcServerListener server;
   struct sigaction sigIntHandler;
   
   
   printf("Server\n");
   
   /*args.addArg("name: HostName, primary: h, alt: host, type: opt, \
               vtype: string, required, desc: Host name/address");
   
   args.addArg("name: Port, primary: p, alt: port, type: opt, \
               vtype: int, required, desc: Set port number");
   
   if (!args.parse((const char**)argv, argc))
   {
      args.printArgErrors(true);
      return 1;
   }
   
   args.getArgVal(l_sHostName, Argument::ArgName, "HostName");
   args.getArgVal(l_nPort, Argument::ArgName, "Port");
    */
   
   sigIntHandler.sa_handler = sighandler;
   sigemptyset(&sigIntHandler.sa_mask);
   sigIntHandler.sa_flags = 0;
   sigaction(SIGINT, &sigIntHandler, NULL);
   
   
   server.setPort(14001);
   server.start();
   
   while (!g_bStopCommanded)
   {
      sleep(1);
   }
   
   server.stop();
   
   return 0;
}

