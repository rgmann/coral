#ifndef CRYPTO_HASH_H
#define CRYPTO_HASH_H

#include "CryptoHashTypes.h"

class CryptoHash
{
public:
   
   CryptoHash(){ m_bValid = false; };
   virtual ~CryptoHash() {};
   
   virtual bool get(Hash128* pHash) = 0;

   virtual bool hashify(unsigned char *pData, unsigned int nBlockSizeBytes) = 0;
   
   bool  isValid() { return m_bValid; };

   virtual void invalidate() { m_bValid = false; };
   
protected:
   
   virtual void compute(unsigned char *pData, unsigned int nBlockSizeBytes) = 0;
   
   void         setValid(){ m_bValid = true; };
   
protected:
   
   bool  m_bValid;
};

#endif // CRYPTO_HASH_H
