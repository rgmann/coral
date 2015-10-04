#ifndef MD5_HASH_H
#define MD5_HASH_H

#include "CryptoHash.h"

class Md5Hash : public CryptoHash
{
public:

   Md5Hash();
   Md5Hash(const Hash128& hash);
   
   bool  hashify(unsigned char *pData, unsigned int nBlockSizeBytes);
   
   bool  get(Hash128* pHash);
   
   Md5Hash & operator=(const Md5Hash &rhs);

   bool operator == (const Md5Hash& other) const;
   bool operator < (const Md5Hash& other) const;
   bool operator <= (const Md5Hash& other) const;
   bool operator > (const Md5Hash& other) const;
   bool operator >= (const Md5Hash& other) const;

private:
   
   void  compute(unsigned char *pData, unsigned int nBlockSizeBytes);
   
private:
   
   Hash128  m_Hash;
};

#endif // MD5_HASH_H