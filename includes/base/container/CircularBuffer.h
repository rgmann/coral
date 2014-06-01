#ifndef CIRCULAR_BUFFER_H
#define CIRCULAR_BUFFER_H

#include <istream>

class CircularBuffer
{
public:
   
   CircularBuffer();
   
   ~CircularBuffer();
   
   bool           allocate(unsigned int nCapacity);
   
   unsigned int   write(const char* pData, unsigned int nBytes);
   unsigned int   write(std::istream& stream, unsigned int nMaxBytes);
   
   unsigned int   read(char* pData, unsigned int nMaxBytes);
   
   /**
    * Read data without removing it from the buffer.
    */
   unsigned int   peek(char* pData, unsigned int nMaxBytes);

   bool  isEmpty() const;
   
   bool  isFull() const;

   unsigned int size() const;

   unsigned int capacity() const;

   void clear();

private:
   
   void add(unsigned int& nPos, unsigned int nAdj);

   unsigned int freeSize() const;

   unsigned int linearFreeSize();

   unsigned int linearUsedSize(unsigned int nReadPos);

   unsigned int allocatedSize() const;

private:

   char* mpBuffer;
   
   unsigned int mnHead;
   
   unsigned int mnTail;
   
   unsigned int mnCapacity;
};

#endif // CIRCULAR_BUFFER_H
