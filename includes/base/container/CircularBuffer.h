#ifndef CIRCULAR_BUFFER_H
#define CIRCULAR_BUFFER_H

class CircularBuffer
{
public:
   
   CircularBuffer();
   
   ~CircularBuffer();
   
   bool           allocate(unsigned int nMaxBytes);
   
   unsigned int   write(const char* pData,
                        unsigned int nBytes,
                        bool bOverwrite = true);
   
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
   
   void initPeekPos();
   
   char peekExtractByte();
   
   char isPeekEmpty() const;
   
   unsigned int increment(unsigned int &counter);
   
private:
   
   unsigned int m_nHead;
   
   unsigned int m_nTail;
   
   unsigned int m_nCapacity;
   
   unsigned int m_nCount;
   
   unsigned int m_nPeekPos;
   
   unsigned int m_nPeekCount;
   
   char* m_pBuffer;
};

#endif // CIRCULAR_BUFFER_H