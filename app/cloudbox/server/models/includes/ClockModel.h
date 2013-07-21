#ifndef CLOCK_MODEL_H
#define CLOCK_MODEL_H

#include "GenericModel.h"

class ClockModel : public GenericModel
{
public:
      
   virtual bool increment() = 0;
   
   virtual ui64 clock() = 0;
};

#endif // CLOCK_MODEL_H