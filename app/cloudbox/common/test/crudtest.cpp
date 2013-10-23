#include <iostream>
#include "JsonObject.h"


int main()
{
   JsonObject lObject;
   JsonObject lNameObject;

   std::cout << "CRUD test:" << std::endl;
   
   lObject << JsonElement("command", std::string("create_user"));

   lNameObject << JsonElement("first", std::string("Robert"));
   lNameObject << JsonElement("last", std::string("Glissmann"));
   lObject << JsonElement("name", lNameObject);

   lObject << JsonElement("age", 25);
   lObject << JsonElement("online", false);
   lObject << JsonElement("numfiles", 30);

//   std::cout << "CrudObject: " << lNameObject.print() << std::endl;
   //lNameObject.printFormatted();
   lObject.printFormatted();
   
   JsonElement lElement;
   if (lObject.getElement("age", lElement))
   {
      int lnAge = 0;
      lElement.getValue(lnAge);
      std::cout << "Age: " << lnAge << std::endl;
   }
   else {
      std::cout << "Failed to get age element." << std::endl;
   }
   
   std::cout << "Encoded: " << lObject.encode() << std::endl;

   lObject.dumpBinary();

   return 0;
}

