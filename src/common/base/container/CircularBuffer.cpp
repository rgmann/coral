#include <stdlib.h>
#include <stdio.h>
#include "CircularBuffer.h"

//------------------------------------------------------------------------------
CircularBuffer::CircularBuffer()
{
   m_pBuffer = NULL;
   
   // Zero is an invalid offset
   m_nHead = m_nTail = 0;
   
   m_nCapacity = 0;
   
   m_nCount = 0;
}

//------------------------------------------------------------------------------
CircularBuffer::~CircularBuffer()
{
   if (m_pBuffer)
   {
      delete[] m_pBuffer;
      m_pBuffer = NULL;
   }
   
   m_nHead = m_nTail = 0;
   
   m_nCapacity = 0;
}

//------------------------------------------------------------------------------
bool CircularBuffer::allocate(unsigned int nMaxBytes)
{
   m_pBuffer = new char[nMaxBytes];
   
   m_nCapacity = nMaxBytes;
   
   return (m_pBuffer != NULL);
}

//------------------------------------------------------------------------------
unsigned int CircularBuffer::write(const char* pData,
                                   unsigned int nBytes,
                                   bool bOverwrite)
{
   unsigned int l_nBytesWritten = 0;
   char l_cTempChar;
   unsigned int l_nBytesNeeded = 0;
   unsigned int l_nReadCount = 0;
   
   if (m_pBuffer == NULL)
   {
      return 0;
   }
   
   // If forced, compute how much space is required and free up the space
   // if possible.
   if (bOverwrite && nBytes > (m_nCapacity - m_nCount))
   {
      l_nBytesNeeded = nBytes - (m_nCapacity - m_nCount);
      
      if (l_nBytesNeeded > m_nCount)
      {
         l_nBytesNeeded = m_nCount;
      }
      
      // Free the bytes by reading.
      for (l_nReadCount = 0;
           l_nReadCount < l_nBytesNeeded;
           l_nReadCount++)
      {
         read(&l_cTempChar, 1);
      }
   }
      
   while (!isFull() && l_nBytesWritten < nBytes)
   {
      insertByte(pData[l_nBytesWritten++]);
   }
   
//   printf("write: cnt=%u, cap=%u, head=%u, tail=%u, needed=%u, wrttn=%u\n",
//          m_nCount, m_nCapacity, m_nHead, m_nTail,
//          l_nBytesNeeded, l_nBytesWritten);
   
   return l_nBytesWritten;
}

//------------------------------------------------------------------------------
unsigned int CircularBuffer::read(char* pData, unsigned int nMaxBytes)
{
   unsigned int l_nBytesRead = 0;
   
   if (m_pBuffer == NULL)
   {
      return 0;
   }
   
//   printf("read: tail=%d, data[0]=%c\n", m_nTail, m_pBuffer[m_nTail]);
   
   while (!isEmpty() && l_nBytesRead < nMaxBytes)
   {
      pData[l_nBytesRead++] = extractByte();
   }
   
   return l_nBytesRead;
}

//------------------------------------------------------------------------------
unsigned int CircularBuffer::peek(char* pData, unsigned int nMaxBytes)
{
   unsigned int l_nBytesRead = 0;
   
   if (m_pBuffer == NULL)
   {
      return 0;
   }
   
   initPeekPos();
   
//   printf("peek: peekPos=%d, max=%u, data0=",
//          m_nPeekPos, nMaxBytes);
   
   while (!isPeekEmpty() && l_nBytesRead < nMaxBytes)
   {
      pData[l_nBytesRead++] = peekExtractByte();
   }
   //printf("\n");
   
   return l_nBytesRead;
}

//------------------------------------------------------------------------------
bool CircularBuffer::isFull() const
{
   return (m_nCount == m_nCapacity);
}

//------------------------------------------------------------------------------
bool CircularBuffer::isEmpty() const
{
   return (m_nCount == 0);
}

//------------------------------------------------------------------------------
void CircularBuffer::insertByte(char byte)
{
   m_pBuffer[increment(m_nHead)] = byte;
   m_nCount++;
}

//------------------------------------------------------------------------------
char CircularBuffer::extractByte()
{
   char l_cTempChar = m_pBuffer[increment(m_nTail)];
   m_nCount--;
   return l_cTempChar;
}

//------------------------------------------------------------------------------
void CircularBuffer::initPeekPos()
{
   m_nPeekPos   = m_nTail;
   m_nPeekCount = m_nCount;
}

//------------------------------------------------------------------------------
char CircularBuffer::peekExtractByte()
{
   char l_cTempChar = m_pBuffer[increment(m_nPeekPos)];
   m_nPeekCount--;
   return l_cTempChar;
}

//------------------------------------------------------------------------------
char CircularBuffer::isPeekEmpty() const
{
   return (m_nPeekCount == 0);
}

//------------------------------------------------------------------------------
unsigned int CircularBuffer::increment(unsigned int &counter)
{
   unsigned int l_nCounter = counter;
   
   counter = (counter + 1) % m_nCapacity;
   
   return l_nCounter;
}
