//#define MONGO_EXPOSE_MACROS

#include <iostream>
#include "mongo/client/dbclient.h"

using namespace std;
using namespace mongo;
int main(){
    DBClientConnection conn;
    conn.connect("localhost:27017");

    BSONObj res;

    assert(conn.runCommand("test", BSON("drop" << "foo"), res));

    conn.insert("test.foo", BSON("_id" << 1 << "a" << BSON_ARRAY(1 << 2 << 3)));

    BSONObjBuilder b; 
    b << "findandmodify" << "foo" << 
         "query" << BSON("a" << 2) << 
         "update" << BSON("$set" << BSON("a.$" << 5)) << 
         "new" << 1;

    assert(conn.runCommand("test", b.obj(), res));

    //PRINT(res.toString());
   cout << res.toString() << endl;

   return 0;
}
