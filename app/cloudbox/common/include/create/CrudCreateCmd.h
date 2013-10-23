#ifndef CREATE_MSG_H
#define CREATE_MSG_H

class CreateMsg : public GenericPacket
{
public:

   enum Type
   {
      TypeNotSet
   }

   struct __attribute__ ((__packed__)) Data
   {
      ui32 type;
      ui32 length;
   };

   CreateMsg();

   CreateMsg(Type type, ui32 nDataSize);
   
   bool  unpack(void* pPkt, ui32 nSizeBytes);

private:
   
   typedef GenericPacket inherited;
};

#endif // CREATE_MSG_H
