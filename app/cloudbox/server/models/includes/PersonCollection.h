#ifndef PERSON_COLLECTION_H
#define PERSON_COLLECTION_H

#include "Collection.h"
#include "Person.h"

class PersonCollection : public Collection
{
public:
   
   PersonCollection(MongoConnection &connection);
   
protected:
   
   virtual GenericModel* createModel();
   
   virtual GenericModel* createModel(const mongo::BSONObj &object);
};

#endif // PERSON_COLLECTION_H