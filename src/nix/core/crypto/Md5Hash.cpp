// 
// Copyright (c) 2015, Robert Glissmann
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// 
// * Redistributions of source code must retain the above copyright notice, this
// list of conditions and the following disclaimer.
// 
// * Redistributions in binary form must reproduce the above copyright notice,
// this list of conditions and the following disclaimer in the documentation
// and/or other materials provided with the distribution.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// 

// %% license-end-token %%
// 
// Author: Robert.Glissmann@gmail.com (Robert Glissmann)
// 
// 



#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include "md5.h"
#include "Md5Hash.h"

static const unsigned int k[64] = {
   0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee ,
   0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501 ,
   0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be ,
   0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821 ,
   0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa ,
   0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8 ,
   0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed ,
   0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a ,
   0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c ,
   0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70 ,
   0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05 ,
   0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665 ,
   0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039 ,
   0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1 ,
   0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1 ,
   0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391 };

// leftrotate function definition
#define LEFTROTATE(x, c) (((x) << (c)) | ((x) >> (32 - (c))))

void PrintSegMd5(unsigned char* pBlock, int nBytes)
{
   for (int ind = 0; ind < nBytes; ind++)
      printf("%d ", *(pBlock + ind));
   printf("\n");
}

//------------------------------------------------------------------------------
Md5Hash::Md5Hash() : CryptoHash()
{
   memset(m_Hash.b, 0, sizeof(Hash128));
}

//------------------------------------------------------------------------------
Md5Hash::Md5Hash(const Hash128& hash)
{
   memcpy(m_Hash.b, hash.b, sizeof(Hash128));
   setValid();
}

//------------------------------------------------------------------------------
bool Md5Hash::hashify(unsigned char *pData, unsigned int nBlockSizeBytes)
{
   compute(pData, nBlockSizeBytes);
   
   setValid();
   
   return true;
}

//------------------------------------------------------------------------------
bool Md5Hash::get(Hash128* pHash) const
{
   if (isValid())
   {
      memcpy(pHash, &m_Hash, sizeof(Hash128));
   }
   
   return isValid();
}

//------------------------------------------------------------------------------
Md5Hash& Md5Hash::operator=(const Md5Hash &rhs)
{
   // Check for self-assignment!
   if (this == &rhs)      // Same object?
      return *this;        // Yes, so skip assignment, and just return *this.
   
   m_bValid = rhs.m_bValid;
   
   if (m_bValid)
   {
      memcpy(&m_Hash, &rhs.m_Hash, sizeof(Hash128));
   }
   
   return *this;  // Return a reference to myself.
}

//------------------------------------------------------------------------------
bool Md5Hash::operator == (const Md5Hash& other) const
{
   // return (memcmp(m_Hash.b, other.m_Hash.b, sizeof(Hash128)) == 0);
   for ( int position = 0; position < sizeof(Hash128); position++ )
   {
      if ( m_Hash.b[ position ] != other.m_Hash.b[ position ] ) return false;
   }

   return true;
}

//------------------------------------------------------------------------------
bool Md5Hash::operator < (const Md5Hash& other) const
{
   return (memcmp(m_Hash.b, other.m_Hash.b, sizeof(Hash128)) < 0);
}

//------------------------------------------------------------------------------
bool Md5Hash::operator <= (const Md5Hash& other) const
{
   return (*this == other) || (*this < other);
}

//------------------------------------------------------------------------------
bool Md5Hash::operator > (const Md5Hash& other) const
{
   return (memcmp(m_Hash.b, other.m_Hash.b, sizeof(Hash128)) > 0);
}

//------------------------------------------------------------------------------
bool Md5Hash::operator >= (const Md5Hash& other) const
{
   return (*this == other) || (*this > other);
}

//------------------------------------------------------------------------------
// http://en.wikipedia.org/wiki/Md5
// http://openwall.info/wiki/people/solar/software/public-domain-source-code/md5
void Md5Hash::compute(unsigned char *pData, unsigned int nBlockSizeBytes)
{
   MD5_CTX ctx;
   
   MD5_Init(&ctx);
   MD5_Update(&ctx, pData, nBlockSizeBytes);
   MD5_Final(m_Hash.b, &ctx);
}
