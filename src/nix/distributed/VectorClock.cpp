#include "VectorClock.h"
#include "libjson.h"
#include <iostream>
#include <sstream>

//------------------------------------------------------------------------------
VectorClock::VectorClock()
{
   for (NodeCount locNodeIdx = 0; locNodeIdx < MaxVectorNodes; locNodeIdx++)
   {
      m_Vector[locNodeIdx] = 0;
   }
}

//------------------------------------------------------------------------------
VectorClock::~VectorClock()
{
}

//------------------------------------------------------------------------------
void  VectorClock::initialize(NodeId nodeId, NodeCount nodes)
{
   if (nodes > MaxVectorNodes)
   {
      m_nNodeCount = MaxVectorNodes;
   }
   else
   {
      m_nNodeCount = nodes;
   }
   
   m_nId = nodeId;
}

//------------------------------------------------------------------------------
void  VectorClock::increment()
{
   m_Vector[m_nId]++;
}

//------------------------------------------------------------------------------
void  VectorClock::update(const VectorClock& other)
{
   increment();
   
   for (NodeCount locnNodeIdx = 0; locnNodeIdx < m_nNodeCount; locnNodeIdx++)
   {
      if (m_Vector[locnNodeIdx] < other.m_Vector[locnNodeIdx])
      {
         m_Vector[locnNodeIdx] = other.m_Vector[locnNodeIdx];
      }
   }
}

//------------------------------------------------------------------------------
std::string VectorClock::toJson()
{
   std::stringstream ss;
   JSONNode n;
   
   n.push_back(JSONNode("NDID", m_nId));
   
   n.push_back(JSONNode("NDCNT", m_nNodeCount));
   
   JSONNode vector(JSON_ARRAY);
   vector.set_name("V");
   
   for (NodeCount locnNodeIdx = 0; locnNodeIdx < m_nNodeCount; locnNodeIdx++)
   {
      vector.push_back(JSONNode("", m_Vector[locnNodeIdx]));
   }
   
   n.push_back(vector);
   return n.write_formatted();
}

//------------------------------------------------------------------------------
bool VectorClock::fromJson(const std::string &jsonString)
{
   JSONNode n = libjson::parse(jsonString);
   
   parseNode(n);
   
   return true;
}

//------------------------------------------------------------------------------
void VectorClock::parseNode(const JSONNode &n)
{
   JSONNode::const_iterator i = n.begin();
   
   while (i != n.end())
   {
      // get the node name and value as a string
      std::string node_name = i->name();
      
      if (node_name == "V" && i->type() == JSON_ARRAY)
      {
         JSONNode::const_iterator vectorNode = i->begin();
         NodeCount locnCurrentNode = 0;
         
         while (vectorNode != i->end())
         {
            m_Vector[locnCurrentNode++] = vectorNode->as_int();
            ++vectorNode;
         }
         return;
      }
      
      // recursively call ourselves to dig deeper into the tree
      if (i -> type() == JSON_ARRAY || i -> type() == JSON_NODE)
      {
         parseNode(*i);
      }
      
      // find out where to store the values
      if (node_name == "NDID"){
         m_nId = i->as_int();
      }
      else if (node_name == "NDCNT"){
         m_nNodeCount = i->as_int();
      }
      
      //increment the iterator
      ++i;
   }
}

//------------------------------------------------------------------------------
bool operator== (VectorClock &cVC1, VectorClock &cVC2)
{
   NodeCount locnNumEqual = 0;
   
   if (cVC1.m_nNodeCount != cVC2.m_nNodeCount)
   {
      return false;
   }
   
   for (NodeCount locnNodeIdx = 0; 
        locnNodeIdx < cVC1.m_nNodeCount; 
        locnNodeIdx++)
   {
      if (cVC1.m_Vector[locnNodeIdx] == cVC2.m_Vector[locnNodeIdx])
      {
         locnNumEqual++;
      }
   }
   
   return (locnNumEqual == cVC1.m_nNodeCount);
}

//------------------------------------------------------------------------------
bool operator< (VectorClock &cVC1, VectorClock &cVC2)
{
   // Less than or equal to count
   NodeCount   locLECount = 0;
   
   // Strictly less count
   NodeCount   locSLCount = 0;
   
   if (cVC1.m_nNodeCount != cVC2.m_nNodeCount)
   {
      return false;
   }
   
   for (NodeCount locnNodeIdx = 0; 
        locnNodeIdx < cVC1.m_nNodeCount; 
        locnNodeIdx++)
   {
      if (cVC1.m_Vector[locnNodeIdx] > cVC2.m_Vector[locnNodeIdx])
      {
         return false;
      }
      if (cVC1.m_Vector[locnNodeIdx] <= cVC2.m_Vector[locnNodeIdx])
      {
         locLECount++;
      }
      if (cVC1.m_Vector[locnNodeIdx] < cVC2.m_Vector[locnNodeIdx])
      {
         locSLCount++;
      }
   }
   
   return   (locLECount == cVC1.m_nNodeCount) && 
            (locSLCount >= 1);
}

//------------------------------------------------------------------------------
bool operator> (VectorClock &cVC1, VectorClock &cVC2)
{
   if (cVC1.m_nNodeCount != cVC2.m_nNodeCount)
   {
      return false;
   }
   
   return !(cVC1 == cVC2) && !(cVC1 < cVC2);
}
