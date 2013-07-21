#include "PersonCollection.h"

//------------------------------------------------------------------------------
PersonCollection::PersonCollection(MongoConnection &connection)
: Collection(connection, "persons")
{
}

//------------------------------------------------------------------------------
GenericModel* PersonCollection::createModel()
{
   return new Person();
}

//------------------------------------------------------------------------------
GenericModel* PersonCollection::createModel(const mongo::BSONObj &object)
{
   Person* l_pPerson = new Person();
   l_pPerson->object(object);
   return l_pPerson;
}
