#include "ArgParser.h"
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>

int main(int argc, char *argv[])
{
   ArgParser args;
   
   std::ifstream l_fLeftFile;
   std::ifstream l_fRightFile;
   
   std::string    l_sLeftName;
   std::string    l_sRightName;
   int            l_nOffset = 0;
   int            l_nCount = 0;
   int            l_nByteCount = 0;
   
   char           l_cLeftByte = 0x00;
   char           l_cRightByte = 0x00;
   
   printf("Argtest test\r\n");
   
   args.addArg("name: left, primary: l, alt: left, type: opt, \
               vtype: string, required, desc: Left file name");
   args.addArg("name: right, primary: r, alt: right, type: opt, \
               vtype: string, required, desc: Right file name");
   args.addArg("name: Offset, primary: o, alt: offs, type: opt, \
               vtype: int, required, desc: Offset into file");
   args.addArg("name: Count, primary: c, alt: count, type: opt, \
               vtype: int, required, desc: Number of bytes");
   
   if (!args.parse((const char**)argv, argc))
   {
      args.printArgErrors(true);
      return 1;
   }
   
   args.getArgVal(l_sLeftName, Argument::ArgName, "left");
   args.getArgVal(l_sRightName, Argument::ArgName, "right");
   args.getArgVal(l_nOffset, Argument::ArgName, "Offset");
   args.getArgVal(l_nCount, Argument::ArgName, "Count");
   
   
   l_fLeftFile.open(l_sLeftName.c_str(), std::ios::binary);
   if (!l_fLeftFile.good())
   {
      std::cout << "Failed to open " << l_sLeftName << std::endl;
      return 1;
   }
   
   l_fRightFile.open(l_sRightName.c_str(), std::ios::binary);
   if (!l_fRightFile.good())
   {
      std::cout << "Failed to open " << l_sRightName << std::endl;
      l_fLeftFile.close();
      return 1;
   }
   
   l_fLeftFile.seekg(l_nOffset, l_fLeftFile.beg);
   if (!l_fLeftFile.good())
   {
      std::cout << "Failed to seek to offset in left file." << std::endl;
      l_fLeftFile.close();
      l_fRightFile.close();
      return 1;
   }
   
   l_fRightFile.seekg(l_nOffset, l_fRightFile.beg);
   if (!l_fRightFile.good())
   {
      std::cout << "Failed to seek to offset in left file." << std::endl;
      l_fLeftFile.close();
      l_fRightFile.close();
      return 1;
   }
   
   printf("    | Left  | Right \n");
   printf("--------------------\n");
   
   while (l_nByteCount < l_nCount)
   {
      char l_cMarginByte = ' ';
      
      l_cLeftByte = l_fLeftFile.get();
      if (!l_fLeftFile.good())
      {
         std::cout << "Failed reading from left file at offset" << std::endl;
         break;
      }
      
      l_cRightByte = l_fRightFile.get();
      if (!l_fRightFile.good())
      {
         std::cout << "Failed reading from right file at offset" << std::endl;
         break;
      }
      
      if (l_cRightByte != l_cLeftByte)
      {
         l_cMarginByte = '>';
//         printf("%04d: 0x%02X | 0x%02X\n",
//                l_nOffset + l_nByteCount,
//                (unsigned char)l_cLeftByte,
//                (unsigned char)l_cRightByte);
      }
      
      printf(" %c 0x%02X | 0x%02X\n",
             l_cMarginByte,
             (unsigned char)l_cLeftByte,
             (unsigned char)l_cRightByte);
      
      l_nByteCount++;
   }
   
   l_fLeftFile.close();
   l_fRightFile.close();
   
   return 0;
}
