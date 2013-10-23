#include "Structure.h"
#include "JsonTransportObject.h"

int main()
{
   Structure container("client_info");
   Structure name("name");
   JsonTransportObject object;

   name.set("first", std::string("Robert"));
   name.set("last", std::string("Glissmann"));

   //container.set("first", std::string("Robert"));
   //container.set("last", std::string("Glissmann"));
   container.set("name", name);
   container.set("age", (i32)25);
   container.set("isSmart", false);

   object.from(container);

   std::cout << "\nobject:" << std::endl;
   std::cout << object.getString() << std::endl;
   
   
   Structure client;
   int clientAge;
   bool bIsSmart = false;
   object.to(client);
   
   std::cout << "\nclient:" << std::endl;
   client.get("age", clientAge);
   client.get("isSmart", bIsSmart);
   std::cout << "  age: " << clientAge << std::endl;
   std::cout << "  smart: " << bIsSmart << std::endl;

   return 0;
}

