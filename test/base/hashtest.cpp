#include "SimpleHashTable.h"
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <map>

#define  MAX_ITEMS      256
#define  MAX_STR_LEN    80
#define  POW            8

typedef struct test_struct
{
   unsigned int   key;
   char           value[MAX_STR_LEN];
} test_struct_t;

int main(int argc, char *argv[])
{
   std::map<unsigned int, unsigned int> taken;
   SimpleHashTable<POW, test_struct_t*> table;
   test_struct_t items[MAX_ITEMS];
   unsigned int   l_nModulus = 1 << (2*POW);
   char           l_sTemp[MAX_STR_LEN];
   unsigned int   l_nFoundCount = 0;
   
   printf("HashTable test mod = %u\r\n", l_nModulus);
   
   //srand (time(NULL));
   
   for (unsigned int l_nInd = 0; l_nInd < MAX_ITEMS; ++l_nInd)
   {
      // Generate keys until we find one that isn't taken
      do {
         items[l_nInd].key = rand() % l_nModulus;
      } while (taken.count(items[l_nInd].key) > 0);
      
      snprintf(items[l_nInd].value, MAX_STR_LEN, "ITEM_%d", items[l_nInd].key);
      
      table.add(items[l_nInd].key, &items[l_nInd]);
      
      // Mark the key as taken
      taken[items[l_nInd].key] = 1;
   }
   
   // Test that all elements can be retrieved
   for (unsigned int l_nInd = 0; l_nInd < MAX_ITEMS; ++l_nInd)
   {
      test_struct_t* l_pTS = NULL;
      bool           l_bFound = false;
      
      memset(l_sTemp, 0, MAX_STR_LEN);
      snprintf(l_sTemp, MAX_STR_LEN, "ITEM_%d", items[l_nInd].key);
      
      l_pTS = table.get(items[l_nInd].key, l_bFound);
      
      if (!l_bFound)
      {
         printf("Failed to find %s\n", l_sTemp);
      }
      else
      {
         if (strncmp(l_sTemp, l_pTS->value, MAX_STR_LEN) != 0)
         {
            printf("Returned item has wrong value: %s\n", l_pTS->value);
         }
         else
         {
            l_nFoundCount++;
         }
      }
   }
   printf("Found %d hash table items\n", l_nFoundCount);
   
   return 0;
}
