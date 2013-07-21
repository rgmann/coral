#include <iostream>
#include "MongoConnection.h"
#include "User.h"
#include "Group.h"

void run()
{
   std::vector<GenericModel*> list;
   User query;
   MongoConnection connection("cbox");

   if (!connection.connect())
   {
      std::cout << "Failed to connect to MongoDB!" << std::endl;
      return;
   }

   query.login("rvaughan");

   if (!User::find(&connection, query, list))
   {
      std::cout << "Failed to query " << connection.dbName() << std::endl;
      return;
   }

   if (list.size() > 0)
   {
      std::cout << dynamic_cast<User*>(list[0])->name().first << ", " <<
                   dynamic_cast<User*>(list[0])->name().second << std::endl;
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

