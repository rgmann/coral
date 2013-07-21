#ifndef VECTOR_CLOCK_H
#define VECTOR_CLOCK_H

#include <vector>
#include "ClockModel.h"

typedef std::vector<ClockModel> ClockList;

class VectorClock
{
public:
   
   VectorClock();
   
   virtual bool load() = 0;
   
   /**
    * Test whether two vectors are identical.
    */
   virtual bool operator == (const VectorClock &other) const;
   
   /**
    * Test whether clock "happened before" the other clock.
    * All clocks in other are equal or greater than those in this instance.
    * At least on clock in other must be strictly greater.
    */
   virtual bool operator < (const VectorClock &other) const;
   
   /**
    * Test whether the other clock "happened before" this clock.
    * All clocks in other are equal or less than those in this instance.
    * At least on clock in other must be strictly less.
    */
   virtual bool operator > (const VectorClock &other) const;
   
   /**
    * Test whether two events occurred concurrently indicating a possible
    * conflict.  Concurrency 
    */
   virtual bool operator || (const VectorClock &other) const;
   
protected:
   
   virtual bool findClock(const ClockList &list, const mongo::OID &query);
   
protected:
   
   ClockList myClock;
};

#endif // VECTOR_CLOCK_H