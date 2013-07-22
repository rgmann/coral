#include <iostream>
#include "MongoConnection.h"
#include "FileCollection.h"
#include "UserCollection.h"

bool createTestFiles(MongoConnection &connection);
bool cleanUp(FileCollection &connection);

void run()
{
   std::vector<GenericModel> list;
   MongoConnection connection("cbox");
   FileCollection collection(connection);
   
   std::cout << "File Collection Test" << std::endl;

   if (!connection.connect())
   {
      std::cout << "Failed to connect to MongoDB!" << std::endl;
      return;
   }

   if (!createTestFiles(connection))
   {
      std::cout << "Failed to insert test files!" << std::endl;
      return;
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

//------------------------------------------------------------------------------
bool createTestFiles(MongoConnection &connection)
{
   bool lbSuccess = false;
   FilePath lFilePath("/home/knoise/cloudbox/myfiles/picture.png");
   
   FileCollection files(connection);
   UserCollection users(connection);
   
   File file;
   User user;
   
   lbSuccess = users.findOne(BSON("name.first" << "Kenneth" <<
                                  "name.last" << "Noisewater"),
                             user);
   if (!lbSuccess) return false;
   
   files.createFile(user, lFilePath, File::Regular, file);
   
   return true;
}
