//#include "FileNode.h"
#include "DsvFileWriter.h"
#include "SerialPort.h"
#include "ArgParser.h"
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string>

bool  g_bStopCommanded = false;

void sighandler(int s)
{
   g_bStopCommanded = true;
   
   exit(1);
}

void PrintVector(const std::vector<std::string> &vec, char delim);

int main(int argc, char *argv[])
{
   //struct sigaction sigIntHandler;
   
   std::vector<std::string> row;
   std::string    path;
   int            l_nLineCount = 0;
   
   DsvFileWriter  fileWriter;
   ArgParser      args;
   
   std::string    l_sPortName = "";
   std::string    l_sHeader   = "";
   std::string    l_sDataLine = "";
   SerialPort     port;
   SerialPort::PortSettings portSettings;
   
   args.addArg("p", ArgParser::ArgOption, ArgParser::ArgRequired, 
               ArgParser::ArgString, "port", "PortName", "Serial port name");
   args.addArg("o", ArgParser::ArgOption, ArgParser::ArgRequired, 
               ArgParser::ArgString, "out", "OutPath", "Path to output file.");
   args.addArg("h", ArgParser::ArgOption, ArgParser::ArgRequired, 
               ArgParser::ArgString, "header", "Header", 
               "Specify header as comma delimited string.");
   args.addArg("v", ArgParser::ArgFlag, ArgParser::ArgOptional, 
               ArgParser::ArgString, "verbose", "VerboseMode", 
               "Specify header as comma delimited string.");
   
   printf("DSV capture:\r\n");
   
   if (!args.parse((const char**)argv, argc))
   {
      args.printArgErrors(true);
      return 1;
   }
   
   // Set port configurations
   portSettings.baud = SerialPort::Baud9600;
   portSettings.mode = SerialPort::Mode8N1;
   
   // Get the port name specified by the user.
   args.getArgVal(l_sPortName, ArgParser::ArgName, "PortName");
   
   if (!port.openPort(l_sPortName.c_str(), portSettings))
   {
      printf("Failed to open specified port.\n");
      return 1;
   }
   
   // Get the header argument.
   args.getArgVal(l_sHeader, ArgParser::ArgName, "Header");
   
   if (!fileWriter.open(path.c_str(), l_sHeader, ','))
   {
      printf("Failed to open file.\n");
      return 1;
   }
   
   // Register the signal handler
   signal(SIGINT, sighandler);
   signal(SIGTERM, sighandler);
   
   while (!g_bStopCommanded)
   {
      if (port.getline(l_sDataLine, 256, 1000) > 0)
      {
         fileWriter.writeRow(l_sDataLine);
         
         if (args.isSet(ArgParser::ArgName, "VerboseMode"))
         {
            printf("%3d: %s\r", l_nLineCount++, l_sDataLine.c_str());
         }
         else
         {
            printf("Line Count: %d\r", l_nLineCount++);
         }
      }
   }
   
   printf("\n\n");
   
   port.closePort();
   fileWriter.close();
   
   return 0;
}

void PrintVector(const std::vector<std::string> &vec, char delim)
{
   std::vector<std::string>::const_iterator it = vec.begin();
   
   if (vec.empty()) {
      return;
   }
   
   for (; it < vec.end(); ++it) {
      printf("%s%c", it->c_str(), delim);
   }
   printf("\n");
}
