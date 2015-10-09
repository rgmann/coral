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



#include "AsioTcpClient.h"

using boost::asio::ip::tcp;
using namespace liber::netapp;

// //-----------------------------------------------------------------------------
// AsioTcpClient::AsioTcpClient( boost::asio::io_service& io_service )
// :  resolver_( io_service ) {}

// //-----------------------------------------------------------------------------
// AsioTcpClient::~AsioTcpClient() {}

// //-----------------------------------------------------------------------------
// void AsioTcpClient::connect(
//    AsioTcpPacketRouterPtr  router,
//    const std::string&      host,
//    const std::string&      port )
// {
//    boost::asio::ip::tcp::resolver::query    query( host.c_str(), port );
//    boost::asio::ip::tcp::resolver::iterator iterator = resolver_.resolve( query );

//    boost::asio::async_connect(
//       router->socket(),
//       iterator,
//       boost::bind(
//         &AsioTcpPacketRouter::handleConnect,
//         router,
//         boost::asio::placeholders::error
//       )
//    );
// }

//-----------------------------------------------------------------------------
AsioTcpClientRouter::AsioTcpClientRouter( boost::asio::io_service& io_service )
   :  AsioTcpPacketRouter( io_service )
   ,  resolver_( io_service )
   ,  timer_( io_service )
{
}

//-----------------------------------------------------------------------------
AsioTcpClientRouter::~AsioTcpClientRouter() {}

//-----------------------------------------------------------------------------
void AsioTcpClientRouter::connect(
   const std::string&      host,
   const std::string&      port,
   int timeout_secs )
{
   boost::asio::ip::tcp::resolver::query    query( host, port );
   boost::asio::ip::tcp::resolver::iterator iterator = resolver_.resolve( query );

   boost::asio::async_connect(
      socket(),
      iterator,
      boost::bind(
        &AsioTcpClientRouter::handleConnect,
        shared_from_this(),
        boost::asio::placeholders::error
      )
   );

   if ( timeout_secs != kWaitForever )
   {
      timer_.expires_from_now(boost::posix_time::seconds( timeout_secs ));
      timer_.async_wait(boost::bind(
         &AsioTcpClientRouter::handleConnectTimeout,
         shared_from_this(),
         boost::asio::placeholders::error ));
   }
}

//-----------------------------------------------------------------------------
void AsioTcpClientRouter::afterConnect()
{
   timer_.cancel();
}
