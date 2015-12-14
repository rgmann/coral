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



#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <boost/uuid/nil_generator.hpp>
#include "Log.h"
#include "RpcException.h"


#define  DS_STRING_FIELD(field) \
if ( success ) \
{ \
   success = ( stream.read_string( field ) != coral::netapp::SerialStream::ReadFail ); \
   if ( success == false ) \
   { \
      log::error("%s::deserialize: Failure at line %d\n",class_name,__LINE__); \
   } \
}

#define  DS_FIELD(field,code) \
if ( success ) \
{ \
   success = stream.read( field ); \
   if ( success ) \
   { \
      code \
   } \
   else \
   { \
      log::error("%s::deserialize: Failure at line %d\n",class_name,__LINE__); \
   } \
}


using namespace coral;
using namespace coral::rpc;
using namespace coral::netapp;


//-----------------------------------------------------------------------------
TraceFrame::TraceFrame(
   const std::string& resource_name,
   const std::string& method_name,
   const std::string& filename,
   int                line_number
)
   : resource_name_( resource_name )
   , method_name_( method_name )
   , filename_( filename )
   , line_number_( line_number )
{
}

//-----------------------------------------------------------------------------
std::string TraceFrame::toString() const
{
   std::stringstream ss;

   ss << "from " 
      << filename_ << ":" 
      << line_number_ << ":in "
      << resource_name_ << "."
      << method_name_;

   return ss.str();
}

//-----------------------------------------------------------------------------
void TraceFrame::pack(coral::netapp::SerialStream& stream) const
{
   stream.write_string( resource_name_ );
   stream.write_string( method_name_ );
   stream.write_string( filename_ );
   stream.write( (ui32)line_number_ );
}

//-----------------------------------------------------------------------------
bool TraceFrame::unpack(coral::netapp::SerialStream& stream)
{
   const char* class_name = "TraceFrame";
   bool success = true;

   DS_STRING_FIELD( resource_name_ );
   DS_STRING_FIELD( method_name_ );
   DS_STRING_FIELD( filename_ );
   DS_FIELD( line_number_, do {} while(0); );

   return success;
}

//-----------------------------------------------------------------------------
RpcException::RpcException()
   : id( kNoException )
   , reporter(RpcException::kUnknown)
{
}

//-----------------------------------------------------------------------------
const RpcCallInfo& RpcException::callInfo() const
{
   return call_info_;
}

//-----------------------------------------------------------------------------
std::string RpcException::toString() const
{
   std::string error_message;

   error_message += callInfo().resource + "(";

   switch (reporter)
   {
      case kServer: error_message += "Server)"; break;
      case kClient: error_message += "Client)"; break;
      default:     error_message += "Unknown)"; break;
   }

   error_message += "::" + callInfo().action + ": ";
   error_message += ToRpcErrorString(id);

   if ( message.length() > 0 )
   {
      error_message += " > " + message;
   }

   return error_message;
}

//-----------------------------------------------------------------------------
void RpcException::reset()
{
  id = kNoException;
  message = "";

  call_info_.resource = "";
  call_info_.action   = "";
  call_info_.uuid     = boost::uuids::nil_generator()();

  trace_.clear();
}

//-----------------------------------------------------------------------------
void RpcException::pack(coral::netapp::SerialStream& stream) const
{
   stream.write( (ui32)reporter );
   stream.write( (ui32)id );
   stream.write_string( message );

   // Serialize the frame trace only if an exception occurred.
   if (id != kNoException)
   {
      stream.write( (ui8)trace_.size() );

      std::vector<TraceFrame>::const_iterator frame_iterator = trace_.begin();
      for (; frame_iterator != trace_.end(); ++frame_iterator )
      {
         frame_iterator->serialize( stream );
      }
   }
   else
   {
      stream.write( (ui8)0 );
   }
}

//-----------------------------------------------------------------------------
bool RpcException::unpack(coral::netapp::SerialStream& stream)
{
   const char* class_name = "RpcException";
   bool success = true;

   i32 tI32Field = -1;
   DS_FIELD( tI32Field, { reporter = (Reporter)tI32Field; } );
   DS_FIELD( tI32Field, { id = (RpcErrorId)tI32Field; } );
   DS_STRING_FIELD( message );

   ui8 frame_count = 0;
   DS_FIELD( frame_count, do {} while(0); );
   if ( success )
   {      
      for ( ui8 frame_index = 0;
           ( frame_index < frame_count ) && success;
           ++frame_index )
      {
         TraceFrame trace_frame;

         if ( trace_frame.deserialize( stream ) == false )
         {
            log::error("RpcException::deserialize failure at %d\n", __LINE__);
            success = false;
         }
         else
         {
            trace_.push_back( trace_frame );
         }
      }
   }

   return success;
}

//-----------------------------------------------------------------------------
void RpcException::pushFrame( const TraceFrame& frame )
{
   trace_.push_back( frame );
}

//-----------------------------------------------------------------------------
void RpcException::pushTrace( const RpcException& other )
{
   if ( id == kNoException )
   {
      id = other.id;
      message = other.message;
      call_info_ = other.call_info_;

      std::vector<TraceFrame>::const_iterator trace_iterator =
         other.frameTrace().begin();
      for (; trace_iterator != other.frameTrace().end(); ++trace_iterator )
      {
         pushFrame( *trace_iterator );
      }
   }
}

//-----------------------------------------------------------------------------
void RpcException::popFrame() 
{
   if ( id == kNoException )
   {
      trace_.pop_back();
   }
}

//-----------------------------------------------------------------------------
const std::vector<TraceFrame>& RpcException::frameTrace() const
{
   return trace_;
}

//-----------------------------------------------------------------------------
std::vector<std::string> RpcException::textTrace() const 
{
   std::vector<std::string> text_trace;

   if ( id != kNoException )
   {
      std::vector<TraceFrame>::const_reverse_iterator text_trace_iterator =
         trace_.rbegin();

      for (; text_trace_iterator != trace_.rend(); ++text_trace_iterator )
      {
         text_trace.push_back( text_trace_iterator->toString() );
      }
   }

   return text_trace;
}

//-----------------------------------------------------------------------------
std::string RpcException::trace() const
{
   std::stringstream trace_stream;

   if ( id != kNoException )
   {
      std::vector<std::string>           text_trace = textTrace();
      std::vector<std::string>::iterator text_trace_iterator = text_trace.begin();

      trace_stream << "RpcException::"
                   << ToRpcErrorString( id )
                   << ": " << message
                   << " from:" << "\n";

      for (; text_trace_iterator != text_trace.end(); ++text_trace_iterator )
      {
         trace_stream << "  " << *text_trace_iterator << "\n";
      }
   }

   return trace_stream.str();
}

