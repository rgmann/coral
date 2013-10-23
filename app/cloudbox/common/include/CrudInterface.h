#ifndef CRUD_INTERFACE_H
#define CRUD_INTERFACE_H

class CrudInterface
{
public:

   CrudInterface();

   virtual ~CrudInterface();

   bool create(CrudCreateObject &object);
   bool read(CrudReadObject &object);
   bool update(CrudUpdateObject &object);
   bool destroy(CrudDestroyObject &object);
};

#endif // CRUD_INTERFACE_H

