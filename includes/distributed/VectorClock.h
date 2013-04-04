#ifndef VECTOR_CLOCK_H
#define VECTOR_CLOCK_H

#include "Serializable.h"

typedef unsigned int NodeClock;
typedef unsigned int NodeCount;
typedef unsigned int NodeId;

class VectorClock : public Serializable
{
public:
   
   VectorClock();
   
   ~VectorClock();
   
   void  initialize(NodeId nodeId, NodeCount nodes);
   
   void  increment();
   
   void  update(const VectorClock& other);
   
   std::string toJson();
   
   bool        fromJson(const std::string &jsonString);
   
   friend bool operator== (VectorClock &cVC1, VectorClock &cVC2);
   friend bool operator< (VectorClock &cVC1, VectorClock &cVC2);
   friend bool operator> (VectorClock &cVC1, VectorClock &cVC2);
   
   static const NodeCount MaxVectorNodes = 10;
   
private:
   
   /**
    * Private copy constructor.
    */
   VectorClock(const VectorClock &clock);
   
   /**
    * Private assignment operator.
    */
   VectorClock& operator= (const VectorClock &clock);
   
   /**
    *
    */
   void parseNode(const JSONNode &n);
   
private:
   
   NodeClock   m_Vector[MaxVectorNodes];
   
   NodeCount   m_nNodeCount;
   
   NodeId      m_nId;
};

#endif // VECTOR_CLOCK_H