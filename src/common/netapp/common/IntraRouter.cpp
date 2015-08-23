#include "Log.h"
#include "GenericPacket.h"
#include "PacketSubscriber.h"
#include "IntraRouter.h"

using namespace liber;
using namespace liber::netapp;

//-----------------------------------------------------------------------------
IntraRouter::IntraRouter()
: PacketRouter(&mReceiver)
, IThread("IntraRouter")
, mpCounterpart(NULL)
{
}

//-----------------------------------------------------------------------------
IntraRouter::~IntraRouter()
{
}

//-----------------------------------------------------------------------------
void IntraRouter::setCounterpart(IntraRouter* pCounterpart)
{
  mpCounterpart = pCounterpart;
}

//-----------------------------------------------------------------------------
void IntraRouter::run(const bool& bShutdown)
{
   while (!bShutdown)
   {
      PacketContainer* container_ptr = NULL;
      if ( ( container_ptr = mReceiver.pop() ) != NULL )
      {
         if ( mpCounterpart )
         {
            mpCounterpart->routePacket( container_ptr );
         }
         else
         {
           log::error("IntraRouter - no counterpart set\n");
         }

         if ( container_ptr->packet_ptr_ )
         {
            delete container_ptr->packet_ptr_;
            container_ptr->packet_ptr_ = NULL;
         }

         delete container_ptr;
      }
   }
}

//-----------------------------------------------------------------------------
void IntraRouter::routePacket(PacketContainer* pContainer)
{
   publish(
      pContainer->destination_id_,
      pContainer->packet_ptr_->basePtr(),
      pContainer->packet_ptr_->allocatedSize());
}


