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



#include <sstream>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "Log.h"
#include "JobReport.h"

using namespace coral;
using namespace coral::rsync;
using namespace coral::netapp;

void serialize_ts( SerialStream& ctor, const Timestamp& ts )
{
   ui32 temp = ts.seconds();
   ctor.write(temp);
   temp = ts.nanoseconds();
   ctor.write(temp);
}

bool deserialize_ts( SerialStream& dtor, Timestamp& ts )
{
   ui32 temp = 0;
   if (dtor.read(temp) == false)
   {
      log::error("deserialize_ts - Failed deserialize seconds\n");
      return false;
   }
   ts.seconds((time_t)temp);

   if (dtor.read(temp) == false)
   {
      log::error("deserialize_ts - Failed deserialize seconds\n");
      return false;
   }
   ts.nanoseconds(temp);

   return true;
}

//----------------------------------------------------------------------------
SegmentationReport::SegmentationReport()
   :  Serializable()
   ,  status(kRsyncSuccess)
   ,  complete( false )
   ,  segmentSizeBytes(0)
   ,  strideSizeBytes(0)
   ,  segmentCount(0)
{
}

//----------------------------------------------------------------------------
void SegmentationReport::print(std::ostream& stream, ui8 tab_size ) const
{
   char* spaces = new char[ tab_size + 1 ];
   memset( spaces, ' ', tab_size );
   spaces[ tab_size ] = '\0';
   stream << spaces << "segment size (bytes): " << segmentSizeBytes << "\n"; 
   stream << spaces << "stride size (bytes): " << strideSizeBytes << "\n"; 
   stream << spaces << "segment count: " << segmentCount << "\n"; 
   stream << spaces << "start time: " << std::fixed << begin.fseconds() << "\n"; 
   stream << spaces << "end time: " << std::fixed << end.fseconds() << "\n"; 
   delete[] spaces;
}

//----------------------------------------------------------------------------
void SegmentationReport::pack( coral::netapp::SerialStream& ctor ) const
{
   ctor.write((ui32)status);
   ctor.write(complete);
   ctor.write(segmentSizeBytes);
   ctor.write(strideSizeBytes);
   ctor.write(segmentCount);

   serialize_ts(ctor, begin);
   serialize_ts(ctor, end);
}

//----------------------------------------------------------------------------
bool SegmentationReport::unpack( SerialStream& dtor )
{
   ui32 tempStatus = 0;
   if (dtor.read(tempStatus) == false)
   {
      log::error("SegmentationReport::unpack - Failed to read status\n");
      return false;
   }
   status = (RsyncError)tempStatus;

   if (dtor.read(complete) == false)
   {
      log::error("SegmentationReport::unpack - Failed to read completion flag\n");
      return false;
   }

   if (dtor.read(segmentSizeBytes) == false)
   {
      log::error("SegmentationReport::unpack - Failed to read rcv segment size\n");
      return false;
   }

   if (dtor.read(strideSizeBytes) == false)
   {
      log::error("SegmentationReport::unpack - Failed to read stride size\n");
      return false;
   }

   if (dtor.read(segmentCount) == false)
   {
      log::error("SegmentationReport::unpack - Failed to read segment count\n");
      return false;
   }

   if (deserialize_ts(dtor, begin) == false)
   {
      log::error("SegmentationReport::unpack - Failed to read hashBegin.mSeconds\n");
      return false;
   }

   if (deserialize_ts(dtor, end) == false)
   {
      log::error("SegmentationReport::unpack - Failed to read hashEnd.mSeconds\n");
      return false;
   }

   return true;
}
//----------------------------------------------------------------------------
AssemblyReport::AssemblyReport()
   :  Serializable()
   ,  status(kRsyncSuccess)
   ,  complete( false )
   ,  segmentCount(0)
   ,  chunkCount(0)
{
}

//----------------------------------------------------------------------------
void AssemblyReport::print( std::ostream& stream, ui8 tab_size ) const
{
   char* spaces = new char[tab_size + 1];
   memset(spaces, ' ', tab_size);
   spaces[tab_size] = '\0';
   stream << spaces << "segment count: " << segmentCount << "\n"; 
   stream << spaces << "chunk count: " << chunkCount << "\n"; 
   stream << spaces << "start time: " << std::fixed << begin.fseconds() << "\n"; 
   stream << spaces << "end time: " << std::fixed << end.fseconds() << "\n";
   delete[] spaces;
}

//----------------------------------------------------------------------------
void AssemblyReport::pack(SerialStream& ctor) const
{
   ctor.write((ui32)status);
   ctor.write(complete);
   ctor.write(segmentCount);
   ctor.write(chunkCount);
   serialize_ts(ctor, begin);
   serialize_ts(ctor, end);
}

//----------------------------------------------------------------------------
bool AssemblyReport::unpack(SerialStream& dtor)
{
   ui32 tempStatus = 0;
   if (dtor.read(tempStatus) == false)
   {
      log::error("AssemblyReport::unpack - Failed to read status\n");
      return false;
   }
   status = (RsyncError)tempStatus;

   if (dtor.read(complete) == false)
   {
      log::error("AssemblyReport::unpack - Failed to read completion flag\n");
      return false;
   }

   if (dtor.read(segmentCount) == false)
   {
      log::error("AssemblyReport::unpack - Failed to read rcv segment count\n");
      return false;
   }

   if (dtor.read(chunkCount) == false)
   {
      log::error("AssemblyReport::unpack - Failed to read chunk count\n");
      return false;
   }

   if (deserialize_ts(dtor, begin) == false)
   {
      log::error("AssemblyReport::unpack - Failed to read hashBegin.mSeconds\n");
      return false;
   }

   if (deserialize_ts(dtor, end) == false)
   {
      log::error("AssemblyReport::unpack - Failed to read hashEnd.mSeconds\n");
      return false;
   }

   return true;
}


//----------------------------------------------------------------------------
AuthorityReport::AuthorityReport()
   :  Serializable()
   ,  status(kRsyncSuccess)
   ,  complete( false )
   ,  receivedSegmentCount(0)
   ,  matchedSegmentCount(0)
   ,  chunkCount(0)
{
}

//----------------------------------------------------------------------------
void AuthorityReport::print(std::ostream& stream, ui8 tab_size) const
{
   char* spaces = new char[tab_size + 1];
   memset(spaces, ' ', tab_size);
   spaces[tab_size] = '\0';
   stream << spaces << "segments received: " << receivedSegmentCount << "\n"; 
   stream << spaces << "segments matched: " << matchedSegmentCount << "\n"; 
   stream << spaces << "chunks sent: " << chunkCount << std::endl; 
   stream << spaces << "hash start time: " << std::fixed << hashBegin.fseconds() << "\n";
   stream << spaces << "hash end time: " << std::fixed << hashEnd.fseconds() << "\n";
   stream << spaces << "auth start time: " << std::fixed << authBegin.fseconds() << "\n";
   stream << spaces << "auth end time: " << std::fixed << authEnd.fseconds() << "\n";
   
   delete[] spaces;
}

//----------------------------------------------------------------------------
void AuthorityReport::pack(SerialStream& ctor) const
{
   ctor.write((ui32)status);
   ctor.write(complete);
   ctor.write(receivedSegmentCount);
   ctor.write(matchedSegmentCount);
   ctor.write(chunkCount);

   serialize_ts(ctor, hashBegin);
   serialize_ts(ctor, hashEnd);
   serialize_ts(ctor, authBegin);
   serialize_ts(ctor, authEnd);
}

//----------------------------------------------------------------------------
bool AuthorityReport::unpack( SerialStream& dtor )
{
   ui32 tempStatus = 0;
   if (dtor.read(tempStatus) == false)
   {
      log::error("AuthorityReport::unpack - Failed to read status\n");
      return false;
   }
   status = (RsyncError)tempStatus;

   if (dtor.read(complete) == false)
   {
      log::error("AuthorityReport::unpack - Failed to read completion flag\n");
      return false;
   }

   if (dtor.read(receivedSegmentCount) == false)
   {
      log::error("AuthorityReport::unpack - "
               "Failed to read rcv segment count\n");
      return false;
   }

   if (dtor.read(matchedSegmentCount) == false)
   {
      log::error("AuthorityReport::unpack - "
               "Failed to read match segment count\n");
      return false;
   }

   if (!dtor.read(chunkCount))
   {
      log::error("AuthorityReport::unpack - Failed to read chunk count\n");
      return false;
   }

   if (deserialize_ts(dtor, hashBegin) == false)
   {
      log::error("AuthorityReport::unpack - Failed to read hashBegin\n");
      return false;
   }

   if (deserialize_ts(dtor, hashEnd) == false)
   {
      log::error("AuthorityReport::unpack - Failed to read hashEnd\n");
      return false;
   }

   if (deserialize_ts(dtor, authBegin) == false)
   {
      log::error("AuthorityReport::unpack - Failed to read authBegin\n");
      return false;
   }

   if (deserialize_ts(dtor, authEnd) == false)
   {
      log::error("AuthorityReport::unpack - Failed to read authEnd\n");
      return false;
   }

   return true;
}

//----------------------------------------------------------------------------
void DestinationReport::pack( SerialStream& ctor ) const
{
   segmentation.serialize(ctor);
   assembly.serialize(ctor);
}

//----------------------------------------------------------------------------
bool DestinationReport::unpack(coral::netapp::SerialStream& dtor)
{
   if (segmentation.deserialize(dtor) == false)
   {
      log::error("DestinationReport::deserialize - "
               "Failed to deserialize dest seg report\n");
      return false;
   }

   if (assembly.deserialize(dtor) == false)
   {
      log::error("JobReport::deserialize - "
               "Failed to deserialize dest assembly report\n");
      return false;
   }

   return true;
}

//----------------------------------------------------------------------------
void SourceReport::pack(coral::netapp::SerialStream& ctor) const
{
   segmentation.serialize(ctor);
   authority.serialize(ctor);
}

//----------------------------------------------------------------------------
bool SourceReport::unpack(coral::netapp::SerialStream& dtor)
{
   if (segmentation.deserialize(dtor) == false)
   {
      log::error("SourceReport::deserialize - "
               "Failed to deserialize source seg report\n");
      return false;
   }

   if (authority.deserialize(dtor) == false)
   {
      log::error("SourceReport::deserialize - "
               "Failed to deserialize source auth report\n");
      return false;
   }

   return true;
}

//----------------------------------------------------------------------------
JobReport::JobReport()
   :  Serializable()
{
}

//----------------------------------------------------------------------------
void JobReport::print( std::ostream& stream ) const
{
   stream << "RSYNC Job Report:\n"
          << " Destination Segmentation Report:\n";
   destination.segmentation.print( stream, 2 );

   stream << " Source Segmentation Report:\n";
   source.segmentation.print( stream, 2 );

   stream << " Source Authority Report:\n";
   source.authority.print( stream, 2 );

   stream << " Destination Assembly Report:\n";
   destination.assembly.print( stream, 2 );
}

//----------------------------------------------------------------------------
std::string JobReport::toString() const
{
   std::stringstream stream;

   print( stream );

   return stream.str();
}

//----------------------------------------------------------------------------
void JobReport::pack(coral::netapp::SerialStream& ctor) const
{
   destination.segmentation.serialize(ctor);
   destination.assembly.serialize(ctor);
   source.segmentation.serialize(ctor);
   source.authority.serialize(ctor);
}

//----------------------------------------------------------------------------
bool JobReport::unpack(coral::netapp::SerialStream& dtor)
{
   if (destination.deserialize(dtor) == false)
   {
      log::error("JobReport::deserialize - "
               "Failed to deserialize dest report\n");
      return false;
   }

   if (source.deserialize(dtor) == false)
   {
      log::error("JobReport::deserialize - "
               "Failed to deserialize source report\n");
      return false;
   }

   return true;
}
