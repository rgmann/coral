#ifndef MONGO_CONNECTION_H
#define MONGO_CONNECTION_H

#include <string>
#include "mongo/client/dbclient.h"

class MongoConnection //: public GenericDbConnection
{
public:
   
   MongoConnection(const std::string &dbName = "");
   
   ~MongoConnection();
   
   bool connect();

   bool disconnect();
   
   bool isConnected() const;
   
   std::string dbName() const;
   
   mongo::DBClientConnection& mongo();

private:
   
   mongo::DBClientConnection   m_dbConnection;
   
   std::string          m_sDbName;
   
   bool m_bIsConnected;
};

#endif // MONGO_CONNECTION_H
