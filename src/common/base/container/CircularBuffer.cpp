#include <stdlib.h>
#include <stdio.h>
#include "Log.h"
#include "CircularBuffer.h"

using namespace liber;

//------------------------------------------------------------------------------
CircularBuffer::CircularBuffer()
: mpBuffer(NULL)
, mnHead(1)
, mnTail(mnHead)
, mnCapacity(0)
{
}

//------------------------------------------------------------------------------
CircularBuffer::~CircularBuffer()
{
  if (mpBuffer)
  {
    delete[] mpBuffer;
    mpBuffer = NULL;
  }
}

//------------------------------------------------------------------------------
bool CircularBuffer::allocate(unsigned int nCapacity)
{
  mnCapacity = nCapacity;
  mpBuffer = new char[mnCapacity + 1];
  return (mpBuffer != NULL);
}

//------------------------------------------------------------------------------
unsigned int CircularBuffer::write(const char* pData, unsigned int nBytes)
{
  unsigned int lnBytesWritten = 0;
   
  if (mpBuffer)
  {
    while (!isFull() && (lnBytesWritten < nBytes))
    {
      unsigned int lnBytesToWrite = linearFreeSize();

      if (lnBytesToWrite > (nBytes - lnBytesWritten))
      {
        lnBytesToWrite = (nBytes - lnBytesWritten);
      }

      memcpy(&mpBuffer[mnHead], &pData[lnBytesWritten], lnBytesToWrite);
      add(mnHead, lnBytesToWrite);
 
      lnBytesWritten += lnBytesToWrite;
    }
  }
   
//   printf("write: cnt=%u, cap=%u, head=%u, tail=%u, needed=%u, wrttn=%u\n",
//          m_nCount, mnCapacity, mnHead, mnTail,
//          l_nBytesNeeded, l_nBytesWritten);
   
   return lnBytesWritten;
}

//------------------------------------------------------------------------------
unsigned int CircularBuffer::write(std::istream& stream, unsigned int nBytes)
{
   unsigned int lnBytesWritten = 0;

   if (mpBuffer)
   {
     while (!isFull() && (lnBytesWritten < nBytes))
     {
       unsigned int lnBytesToWrite = linearFreeSize();
//       log::status("write: lnBytesToWrite=%u, nBytes=%u, lnBytesWritten=%u, mnHead=%u, mnTail=%u\n", lnBytesToWrite, nBytes, lnBytesWritten, mnHead, mnTail);

       if (lnBytesToWrite > (nBytes - lnBytesWritten))
       {
          lnBytesToWrite = (nBytes - lnBytesWritten);
       }

       stream.read(&mpBuffer[mnHead], lnBytesToWrite);
       add(mnHead, stream.gcount());
       lnBytesWritten += stream.gcount();
       if (stream.eof())
       {
         break;
       }
       if (stream.fail())
       {
         log::error("write: stream read failure - attempted %u, atual %u\n", lnBytesToWrite, stream.gcount());
       }
     }
   }

   return lnBytesWritten;
}

//------------------------------------------------------------------------------
unsigned int CircularBuffer::read(char* pData, unsigned int nMaxBytes)
{
   unsigned int lnBytesRead = 0;
   
   if (mpBuffer)
   {
     while (!isEmpty() && (lnBytesRead < nMaxBytes))
     {
       unsigned int lnBytesToRead = linearUsedSize(mnTail);
//       log::status("read: lnBytesToRead=%u, nMaxBytes=%u, lnBytesRead=%u, mnHead=%u, mnTail=%u\n", lnBytesToRead, nMaxBytes, lnBytesRead, mnHead, mnTail);

       if (lnBytesToRead == 0) break;

       if (lnBytesToRead > (nMaxBytes - lnBytesRead))
       {
         lnBytesToRead = (nMaxBytes - lnBytesRead);
       }

       memcpy(&pData[lnBytesRead], &mpBuffer[mnTail], lnBytesToRead);
       add(mnTail, lnBytesToRead);

       lnBytesRead += lnBytesToRead;
    }
  }
   
  return lnBytesRead;
}

//------------------------------------------------------------------------------
unsigned int CircularBuffer::peek(char* pData, unsigned int nMaxBytes)
{
  unsigned int lnBytesRead = 0;

  if (mpBuffer)
  {
    unsigned int lnPeekPos = mnTail;

    while (lnBytesRead < nMaxBytes)
    {
      unsigned int lnBytesToRead = linearUsedSize(lnPeekPos);
//      log::status("peek: lnBytesToRead=%u, nMaxBytes=%u, lnBytesRead=%u, mnHead=%u, mnTail=%u, peek=%u\n", lnBytesToRead, nMaxBytes, lnBytesRead, mnHead, mnTail, lnPeekPos);

      if (lnBytesToRead == 0) break;

      if (lnBytesToRead > (nMaxBytes - lnBytesRead))
      {
        lnBytesToRead = (nMaxBytes - lnBytesRead);
      }

      memcpy(&pData[lnBytesRead], &mpBuffer[lnPeekPos], lnBytesToRead);
      add(lnPeekPos, lnBytesToRead);

      lnBytesRead += lnBytesToRead;
    }
  }

  return lnBytesRead;
}

//------------------------------------------------------------------------------
bool CircularBuffer::isFull() const
{
   return (((mnHead + 1) % (mnCapacity + 1)) == mnTail);
}

//------------------------------------------------------------------------------
bool CircularBuffer::isEmpty() const
{
   return (mnHead == mnTail);
}

//------------------------------------------------------------------------------
void CircularBuffer::clear()
{
  mnHead = mnTail = 1;
}

//------------------------------------------------------------------------------
unsigned int CircularBuffer::size() const
{
  unsigned int lnSize = mnHead;
  if (mnHead < mnTail) lnSize += allocatedSize();
  lnSize -= mnTail;
  return lnSize;
}

//------------------------------------------------------------------------------
unsigned int CircularBuffer::capacity() const
{
  return mnCapacity;
}

//------------------------------------------------------------------------------
unsigned int CircularBuffer::allocatedSize() const
{
  return mnCapacity + 1;
}

//------------------------------------------------------------------------------
unsigned int CircularBuffer::freeSize() const
{
  return capacity() - size();
}

//------------------------------------------------------------------------------
unsigned int CircularBuffer::linearFreeSize()
{
  unsigned int lnFree = 0;

  if (mnTail <= mnHead)
  {
    lnFree = allocatedSize() - mnHead;
  }
  else
  {
    lnFree = (mnTail - mnHead) - 1;
  }

  return lnFree;
}

//------------------------------------------------------------------------------
unsigned int CircularBuffer::linearUsedSize(unsigned int nReadPos)
{
  unsigned int lnReadable = 0;

  if (nReadPos <= mnHead)
  {
    lnReadable = mnHead - nReadPos;
  }
  else
  {
    lnReadable = allocatedSize() - nReadPos;
  }

  return lnReadable;
}

//----------------------------------------------------------------------------
void CircularBuffer::add(unsigned int& nPos, unsigned int adj)
{
  nPos += adj;
  nPos %= allocatedSize();
}

