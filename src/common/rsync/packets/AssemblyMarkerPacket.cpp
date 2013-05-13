#include "AssemblyMarkerPacket.h"

//------------------------------------------------------------------------------
AssemblyMarkerPacket::AssemblyMarkerPacket()
: RsyncAssemblyInstr(RsyncAssemblyInstr::MarkerType, sizeof(Data))
{
   if (allocate())
   {
      type(TypeNotSet);
   }
}

//------------------------------------------------------------------------------
AssemblyMarkerPacket::AssemblyMarkerPacket(Type type)
: RsyncAssemblyInstr(RsyncAssemblyInstr::MarkerType, sizeof(Data))
{
   if (allocate())
   {
      type(type);
   }
}

//------------------------------------------------------------------------------
AssemblyMarkerPacket::Type AssemblyMarkerPacket::type() const
{
   Type type = TypeNotSet;
   
   if (data())
   {
      Data* const l_pData = reinterpret_cast<Data* const>(data());
      
      type = l_pData->type;
   }
   
   return type;
}

//------------------------------------------------------------------------------
bool AssemblyMarkerPacket::type(Type type)
{
   bool l_bSuccess = false;
   
   if (data())
   {
      Data* const l_pData = reinterpret_cast<Data* const>(data());
      
      l_pData->type = type;
      
      l_bSuccess = true;
   }
   
   return l_bSuccess;
}

//------------------------------------------------------------------------------
void* AssemblyMarkerPacket::dataPtr()
{
   return inherited::dataEndPtr();
}

//------------------------------------------------------------------------------
void* const AssemblyMarkerPacket::data() const
{
   return inherited::dataEndPtr();
}
