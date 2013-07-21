#ifndef PERSON_H
#define PERSON_H

#include "GenericModel.h"

class Person : public GenericModel
{
public:
      
   Person();
   
   ~Person();
   
   void        name(const std::string &name);
   std::string name() const;
   
   void age(int age);
   int age() const;
   
   void        city(const std::string &city);
   std::string city() const;
   
   void        state(const std::string &state);
   std::string state() const;
   
   ui32 visitCount() const;
};

#endif // PERSON_H
