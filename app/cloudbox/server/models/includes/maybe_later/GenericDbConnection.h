#ifndef GENERIC_DB_CONNECTION_H
#define GENERIC_DB_CONNECTION_H

class GenericDbConnection
{
public:
   
   GenericDbConnection();
   
   virtual ~GenericDbConnection();
   
   virtual bool connect();
   
   virtual bool disconnect();
};

#endif // GENERIC_DB_CONNECTION_H
