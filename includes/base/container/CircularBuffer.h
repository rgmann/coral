#ifndef CIRCULAR_BUFFER_H
#define CIRCULAR_BUFFER_H

#include <istream>

class CircularBuffer
{
public:
   
   CircularBuffer();
   
   ~CircularBuffer();
   
   bool           allocate(unsigned int nMaxBytes);
   
   unsigned int   write(const char* pData,
                        unsigned int nBytes,
                        bool bOverwrite = true);
   unsigned int   write(std::istream& stream, unsigned int nMaxBytes, bool bOverwrite = true);
   
   unsigned int   read(char* pData, unsigned int nMaxBytes);
   
   /**
    * Read data without removing it from the buffer.
    */
   unsigned int   peek(char* pData, unsigned int nMaxBytes);

   bool  isEmpty() const;
   
   bool  isFull() const;

private:
   
   void insertByte(char byte);
   
   char extractByte();
   
   unsigned int increment(unsigned int &counter);

   unsigned int getLinearFreeCount();

   unsigned int getLinearUsedCount(unsigned int nReadPos);

private:
   
   unsigned int m_nHead;
   
   unsigned int m_nTail;
   
   unsigned int m_nCapacity;
   
   unsigned int m_nCount;
   
   char* m_pBuffer;
};

#endif // CIRCULAR_BUFFER_H
