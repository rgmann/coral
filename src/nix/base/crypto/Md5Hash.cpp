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
bool Md5Hash::get(Hash128* pHash)
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
   return (memcmp(m_Hash.b, other.m_Hash.b, sizeof(Hash128)) == 0);
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
//void Md5Hash::compute(unsigned char *pData, unsigned int nBlockSizeBytes)
//{
//   // Message (to prepare)
//   unsigned char *msg = NULL;
//   
//   // Note: All variables are unsigned 32 bit and wrap modulo 2^32 when calculating
//   
//   // r specifies the per-round shift amounts
//   const unsigned int r[] = {7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22,
//      5,  9, 14, 20, 5,  9, 14, 20, 5,  9, 14, 20, 5,  9, 14, 20,
//      4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23,
//      6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21};
//   
//   // Initialize variables - simple count in nibbles:
//   m_Hash.w[0] = 0x67452301;
//   m_Hash.w[1] = 0xefcdab89;
//   m_Hash.w[2] = 0x98badcfe;
//   m_Hash.w[3] = 0x10325476;
//   
//   // Pre-processing: adding a single 1 bit
//   //append "1" bit to message    
//   /* Notice: the input bytes are considered as bits strings,
//    where the first bit is the most significant bit of the byte.[37] */
//   
//   // Pre-processing: padding with zeros
//   //append "0" bit until message length in bit ≡ 448 (mod 512)
//   //append length mod (2 pow 64) to message
//   
//   int new_len;
//   for(new_len = nBlockSizeBytes*8 + 1; new_len%512!=448; new_len++);
//   new_len /= 8;
//   printf("newlen: %d\n", new_len);
//   
//   msg = new unsigned char[new_len + 64]; //calloc(new_len + 64, 1); // also appends "0" bits 
//   memset(msg, 0, new_len + 64);
//   
//   // (we alloc also 64 extra bytes...)
//   memcpy(msg, pData, nBlockSizeBytes);
//   msg[nBlockSizeBytes] = 128; // write the "1" bit
//   
//   unsigned int bits_len = 8*nBlockSizeBytes; // note, we append the len
//   memcpy(msg + new_len, &bits_len, 4);           // in bits at the end of the buffer
//   
//   // Process the message in successive 512-bit chunks:
//   //for each 512-bit chunk of message:
//   int offset;
//   for(offset=0; offset<new_len; offset += (512/8)) {
//      
//      // break chunk into sixteen 32-bit words w[j], 0 ≤ j ≤ 15
//      unsigned int *w = (unsigned int*) (msg + offset);
////#define DEBUG
//#ifdef DEBUG
//      printf("offset: %d %x\n", offset, offset);
//      
//      int j;
//      for(j =0; j < 64; j++) printf("%x ", ((unsigned char *) w)[j]);
//      puts("");
//#endif
//      
//      // Initialize hash value for this chunk:
//      unsigned int a = m_Hash.w[0];
//      unsigned int b = m_Hash.w[1];
//      unsigned int c = m_Hash.w[2];
//      unsigned int d = m_Hash.w[3];
//      
//      // Main loop:
//      unsigned int i;
//      for(i = 0; i<64; i++) {
//         
//         unsigned int f, g;
//         
//         if (i < 16) {
//            f = (b & c) | ((~b) & d);
//            g = i;
//         } else if (i < 32) {
//            f = (d & b) | ((~d) & c);
//            g = (5*i + 1) % 16;
//         } else if (i < 48) {
//            f = b ^ c ^ d;
//            g = (3*i + 5) % 16;          
//         } else {
//            f = c ^ (b | (~d));
//            g = (7*i) % 16;
//         }
//         
//         unsigned int temp = d;
//         d = c;
//         c = b;
//         b = b + LEFTROTATE((a + f + k[i] + w[g]), r[i]);
//         a = temp;
//         
//      }
//      
//      // Add this chunk's hash to result so far:
//      m_Hash.w[0] += a;
//      m_Hash.w[1] += b;
//      m_Hash.w[2] += c;
//      m_Hash.w[3] += d;
//      
//   }
//   
//   // cleanup
//   delete[] msg;
//}
