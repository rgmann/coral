#include "Timestamp.h"
#include <string.h>

//------------------------------------------------------------------------------
Timestamp::Timestamp()
{
   memset(&m_TV, 0, sizeof(m_TV));
}

//------------------------------------------------------------------------------
Timestamp::~Timestamp()
{
}

//------------------------------------------------------------------------------
void Timestamp::sample()
{
   gettimeofday(&m_TV, NULL);
}

//------------------------------------------------------------------------------
long Timestamp::diffInS(const Timestamp &start)
{
   return (m_TV.tv_sec - start.m_TV.tv_sec);
}

//------------------------------------------------------------------------------
long Timestamp::diffInMs(const Timestamp &start)
{
   return (m_TV.tv_sec - start.m_TV.tv_sec) * 1000;
}

//------------------------------------------------------------------------------
long Timestamp::diffInUs(const Timestamp &start)
{
   return ((m_TV.tv_sec - start.m_TV.tv_sec) * 1000000) + 
            (m_TV.tv_usec - start.m_TV.tv_usec);
}

