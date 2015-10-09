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



#include "AsioTcpServer.h"

using namespace liber::netapp;

//-----------------------------------------------------------------------------
AsioTcpServer::AsioTcpServer(
   boost::asio::io_service&   io_service,
   const boost::asio::ip::tcp::endpoint&       endpoint )
:  io_service_( io_service )
,  acceptor_  ( io_service, endpoint )
{
}

//-----------------------------------------------------------------------------
AsioTcpServer::~AsioTcpServer()
{
}

//-----------------------------------------------------------------------------
void AsioTcpServer::startAccept()
{
   AsioTcpPacketRouterPtr connection = createRouter( io_service_ ) ;

   acceptor_.async_accept(
      connection->socket(),
      boost::bind(
        &AsioTcpServer::handleAccept,
        this,
        connection,
        boost::asio::placeholders::error
      )
   );
}

//-----------------------------------------------------------------------------
void AsioTcpServer::handleAccept(
   AsioTcpPacketRouterPtr           connection,
   const boost::system::error_code& error )
{
   if ( !error )
   {
      connection->start();
   }

   startAccept();
}
