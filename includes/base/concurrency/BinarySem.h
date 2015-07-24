#ifndef BINARY_SEM_H
#define BINARY_SEM_H

#include "Semaphore.h"

class BinarySem : public liber::thread::Semaphore {
public:
   
  BinarySem() : liber::thread::Semaphore( 0, 1 ) {};

};

#endif // BINARY_SEM_H