#include "ArgParser.h"
#include <stdio.h>

int main(int argc, char *argv[])
{
   ArgParser args;
   
   std::string l_sHostName;
   int         l_nPort = 5000;
   
   printf("Argtest test\r\n");
   
   args.addArg("name: HostName, primary: h, alt: host, type: opt, \
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
   
   printf("HostName: %s, Port: %d\n", l_sHostName.c_str(), l_nPort);
   
   return 0;
}
