#include <iostream>
#include "MongoConnection.h"
#include "PersonCollection.h"

void run()
{
   std::vector<GenericModel> list;
   Person person;
   MongoConnection connection("tutorial");
   PersonCollection collection(connection);

   if (!connection.connect())
   {
      std::cout << "Failed to connect to MongoDB!" << std::endl;
      return;
   }

   if (!collection.findOne(BSON("name" << "Samantha"), person))
   {
      std::cout << "Failed to query " << connection.dbName() << std::endl;
      return;
   }

   std::cout << "Name: " << person.name()
   << ", Age: " << person.age()
   << ", City: " << person.city()
   << ", State: " << person.state()
   << ", OID: " << person.idToString()
   << ", visits: " << person.visitCount()
   << std::endl;
   
   collection.increment(person, "visits");
   
   Person tim;
   
   tim.name("Tim");
   tim.age(32);
   tim.city("London");
   tim.state("ON");
   if (!collection.remove(tim))
   {
      std::cout << "Failed to remove Tim" << std::endl;
   }
}

int main() {
    try {
        run();
    }
   catch( mongo::DBException &e ) {
      std::cout << "caught " << e.what() << std::endl;
    }
    return 0;
}

