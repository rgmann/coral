#include "MongoConnection.h"

//------------------------------------------------------------------------------
MongoConnection::MongoConnection(const std::string &dbName)
: m_sDbName(dbName)
, m_bIsConnected(false)
{
}

//------------------------------------------------------------------------------
MongoConnection::~MongoConnection()
{
}

//------------------------------------------------------------------------------
bool MongoConnection::connect()
{
   m_dbConnection.connect("localhost");
   return (m_bIsConnected = true);
}

//------------------------------------------------------------------------------
bool MongoConnection::disconnect()
{
   return true;
}

//------------------------------------------------------------------------------
bool MongoConnection::isConnected() const
{
   return m_bIsConnected;
}

//------------------------------------------------------------------------------
std::string MongoConnection::dbName() const
{
   return m_sDbName;
}

//------------------------------------------------------------------------------
mongo::DBClientConnection& MongoConnection::mongo()
{
   return m_dbConnection;
}
