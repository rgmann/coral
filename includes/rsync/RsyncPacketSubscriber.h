#ifndef  RSYNC_PACKET_SUBSCRIBER_H
#define  RSYNC_PACKET_SUBSCRIBER_H


#include "PacketSubscriber.h"

namespace liber {
namespace rsync {

class RsyncJob;

class RsyncPacketSubscriber : public liber::netapp::PacketSubscriber {
public:

   RsyncPacketSubscriber( bool require_active_job = true );

   void setActiveJob( RsyncJob* job_ptr );
   void unsetActiveJob();

   //
   // Receive and process a packet for the specified destination. The
   // RsyncPacketSubscriber expects the first sixteen bytes to be occupied by
   // the job UUID.
   //
   bool put( liber::netapp::DestinationID destination_id, const void* data_ptr, ui32 length );


protected:

   //
   // Get a pointer to the active job.
   //
   // @precondition  The active job lock has already been acquired.
   //
   RsyncJob* activeJob();

   void disablePacketRouting();

   boost::mutex& activeJobLock();

   bool sendTo( liber::netapp::DestinationID destination_id, int type, const void* data_ptr, ui32 length );

   virtual bool processPacket( const void* data_ptr, ui32 length ) = 0;

   virtual bool validUuid( const ui8* uuid_ptr );


private:

   bool require_active_job_;

   boost::mutex lock_;

   RsyncJob* active_job_ptr_;

   bool allow_packets_;

};

}
}


#endif  // RSYNC_PACKET_SUBSCRIBER_H
