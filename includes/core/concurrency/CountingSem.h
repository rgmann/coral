#ifndef COUNTING_SEM_H
#define COUNTING_SEM_H

#include "Semaphore.h"

class CountingSem : public liber::thread::Semaphore {
public:
   
   CountingSem( i32 start ) : liber::thread::Semaphore( start ) {};

};

#endif // COUNTING_SEM_H