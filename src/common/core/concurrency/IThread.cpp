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



#include <iostream>
#include "Log.h"
#include "IThread.h"

using namespace liber::concurrency;

ui32 IThread::ournThreadId = 0;

//--------------------------------------------------------------------
IThread::IThread( const std::string& name )
:  thread_id_  ( IThread::ournThreadId++ )
,  name_       ( name )
,  running_    ( false )
{
}

//--------------------------------------------------------------------
IThread::~IThread()
{
   // thread_.interrupt();
   // thread_.join();
}

//--------------------------------------------------------------------
ui32 IThread::getID() const
{
   return thread_id_;
}

//--------------------------------------------------------------------
const std::string& IThread::getName() const
{
   return name_;
}

//-----------------------------------------------------------------------------
bool IThread::launch()
{
   bool launch_success = false;

   if ( running_ == false )
   {
      thread_ = boost::thread( &IThread::entry, this );

      launch_success = true;
   }

   return launch_success;
}

//-----------------------------------------------------------------------------
void IThread::join()
{
   thread_.join();
}

//-----------------------------------------------------------------------------
void IThread::cancel( bool join )
{
   if ( running_ )
   {
      thread_.interrupt();

      if ( join )
      {
         thread_.join();
      }
   }
}

//-----------------------------------------------------------------------------
bool IThread::isRunning() const
{
   return running_;
}

//-----------------------------------------------------------------------------
void IThread::entry()
{
   running_ = true;

   run( boost::this_thread::interruption_requested() );

   running_ = false;
}
