#include <string.h>
#include <iostream>
#include <exception>
#include <boost/lexical_cast.hpp>
#include "Timestamp.h"
#include "client.HeimdallController.h"
//#include "NetAppPacket.h"

#define HC_MARKER  0xFEEDB33F

using namespace eterm;
using namespace rpc_eterm;
//using namespace liber::net;
//using namespace liber::netapp;
using boost::asio::ip::tcp;

//-----------------------------------------------------------------------------
HeimdallController::HeimdallController(const char* pHost, int nPort)
: liber::concurrency::IThread("HeimdallController")
, mpHost(pHost)
, mnPort(nPort)
, mUserDb("data/userdb.bin")
//, mbConnected(false)
{
  mStatus.busy = false;
  mStatus.connected = false;

  mCommands.initialize();
  mUserDb.load();
  launch();
}

//-----------------------------------------------------------------------------
HeimdallController::~HeimdallController()
{
  mUserDb.store();
  join();
}

//------------------------------------------------------------------------------
HeimdallController::Status HeimdallController::
setLedState(LedId id, bool bOn)
{
//  Status lStatus;
//  lStatus.connected = mbConnected;
//  lStatus.busy = mbBusyEnrolling;

//  if (!mbBusyEnrolling)
  if (!mStatus.busy)
  {
    HcMessage lMessage;
    lMessage.header.type = HC_SET_LED_STATE;
    lMessage.header.length = sizeof(HcLedCommand);

    lMessage.data.ledState.ledID = id;
    lMessage.data.ledState.ledOn = bOn;

    mCommands.push(lMessage);
  }

//  return lStatus;
  return mStatus;
}

//------------------------------------------------------------------------------
HeimdallController::Status HeimdallController::
activateDoor()
{
//  Status lStatus;
//  lStatus.connected = mbConnected;
//  lStatus.busy = mbBusyEnrolling;

//  if (!mbBusyEnrolling)
  if (!mStatus.busy)
  {
    HcMessage lMessage;
    lMessage.header.type = HC_ACTIVATE_DOOR;
    lMessage.header.length = 0;

    mCommands.push(lMessage);
  }

//  return lStatus;
  return mStatus;
}

//------------------------------------------------------------------------------
HeimdallController::Status HeimdallController::
enroll(const std::string& first, const std::string& last)
{
//  Status lStatus;
//  lStatus.connected = mbConnected;
//  lStatus.busy = mbBusyEnrolling;

//  if (!mbBusyEnrolling)
  if (!mStatus.busy)
  {
    HcMessage lMessage;
    lMessage.header.type = HC_ENROLL_NEW;
    lMessage.header.length = 0;

    mActiveEnrollMutex.lock();
    mpActiveUser = new User();
    mpActiveUser->mutable_user_name()->set_first_name(first);
    mpActiveUser->mutable_user_name()->set_last_name(last);
    mActiveEnrollMutex.unlock();

    mCommands.push(lMessage);
  }

//  return lStatus;
  return mStatus;
}

//------------------------------------------------------------------------------
HeimdallController::Status HeimdallController::
removeOne(ui8 id)
{
//  Status lStatus;
//  lStatus.connected = mbConnected;
//  lStatus.busy = mbBusyEnrolling;

//  if (!mbBusyEnrolling)
  if (!mStatus.busy)
  {
    HcMessage lMessage;
    lMessage.header.type = HC_REMOVE_ONE;
    lMessage.header.length = sizeof(HcRemoveOne);

    lMessage.data.removeOne.userID = id;

    mCommands.push(lMessage);
  }

//  return lStatus;
  return mStatus;
}

//------------------------------------------------------------------------------
HeimdallController::Status HeimdallController::
removeAll()
{
//  Status lStatus;
//  lStatus.connected = mbConnected;
//  lStatus.busy = mbBusyEnrolling;

//  if (!mbBusyEnrolling)
  if (!mStatus.busy)
  {
    HcMessage lMessage;
    lMessage.header.type = HC_REMOVE_ALL;
    lMessage.header.length = 0;

    mCommands.push(lMessage);
  }

//  return lStatus;
  return mStatus;
}

//------------------------------------------------------------------------------
bool HeimdallController::
sendCommand(boost::asio::ip::tcp::socket& rSocket, const HcMessage& rMessage)
{
  boost::system::error_code ignored_error;

  // Send the packet header.
  char* lpData = (char*)&rMessage.header;
  boost::asio::write(rSocket,
                     boost::asio::buffer(lpData, sizeof(HcMessage::PacketHeader)), 
                     ignored_error);

  if (ignored_error == boost::asio::error::eof)
  {
//    mbConnected = false;
    mStatus.connected = false;
    rSocket.close();
    return false;
  }

  // Send the packet data (if any).
  if (rMessage.header.length > 0)
  {
    lpData = (char*)&rMessage.data;
    boost::asio::write(rSocket,
                       boost::asio::buffer(lpData, rMessage.header.length), 
                       ignored_error);

    if (ignored_error == boost::asio::error::eof)
    {
//      mbConnected = false;
      mStatus.connected = false;
      rSocket.close();
      return false;
    }
  }

  return true;
}

//------------------------------------------------------------------------------
bool HeimdallController::recvResponse(boost::asio::ip::tcp::socket& rSocket, HcMessage& rMessage)
{
  boost::system::error_code ignored_error;

  // Attempt to receive the packet header.
  boost::asio::read(rSocket,
                    boost::asio::buffer(&rMessage.header, sizeof(HcMessage::PacketHeader)),
                    ignored_error);
  if (ignored_error == boost::asio::error::eof)
  {
//    mbConnected = false;
    mStatus.connected = false;
    rSocket.close();
    return false;
  }

  if (rMessage.header.length > 0)
  {
    boost::asio::read(rSocket,
                      boost::asio::buffer(&rMessage.data, rMessage.header.length),
                      ignored_error);
    if (ignored_error == boost::asio::error::eof)
    {
//      mbConnected = false;
      mStatus.connected = false;
      rSocket.close();
      return false;
    }
  }

  return true;
}

//------------------------------------------------------------------------------
void HeimdallController::run(const bool& bShutdown)
{
  boost::asio::io_service io_service;
  boost::asio::ip::tcp::socket lSocket(io_service);

  HcMessage lCommand;
  HcMessage lResponse;

  Timestamp lReconnectTimer;
  Timestamp lPollHeimdallTimer;

  lReconnectTimer.sample();
  lPollHeimdallTimer.sample();

  while (!bShutdown)
  {
//    if (mbConnected)
    if (mStatus.connected)
    {
      lCommand.header.type = 0;

      // If an enrollment command is active, ask for newly enrolled users.
//      if (!mbBusyEnrolling)
      if (!mStatus.busy)
      {
        // If enrollment is not active,
        // we can attempt to pull a command from the queue.
        if (!mCommands.isEmpty())
        {
          mCommands.pop(lCommand);
        }
        else if ((Timestamp::Now() - lPollHeimdallTimer).fseconds() > 2)
        {
          lPollHeimdallTimer.sample();

          // Otherwise check for new identfied users.
          lCommand.header.type = HC_GET_NEW_IDD;
          lCommand.header.length = 0;
        }
      }
      else if ((Timestamp::Now() - lPollHeimdallTimer).fseconds() > 2)
      {
        lPollHeimdallTimer.sample();

        // Ask for newly enrolled user.
        lCommand.header.type = HC_GET_NEW_ENROLLED;
        lCommand.header.length = 0;
      }

      if (lCommand.header.type > 0)
      {
        lCommand.header.marker = HC_MARKER;

        if (lCommand.header.type == HC_ENROLL_NEW)
        {
//          mbBusyEnrolling = true;
          mStatus.busy = true;
        }

        if (sendCommand(lSocket, lCommand))
        {
          if (!recvResponse(lSocket, lResponse))
          {
            printf("HeimdallCtrl: Failed to receive valid response.\n");
          }
          else if (lResponse.header.type != lCommand.header.type)
          {
            printf("HeimdallCtrl: Unexpected response type = %d\n",
                   lResponse.header.type);
          }
          else
          {
            // Handle the response.
            switch (lResponse.header.type)
            {
            case HC_GET_NEW_ENROLLED:
              if (lResponse.data.newIdentified.userID > 0)
              {
                mActiveEnrollMutex.lock();
                User* lpUser = mpActiveUser;
                mpActiveUser = NULL;
                mActiveEnrollMutex.unlock();

                if (lpUser)
                {
                  Timestamp lTime = Timestamp::Now();
                  lpUser->mutable_last_access()->set_seconds(lTime.seconds());
                  lpUser->mutable_last_access()->set_nanoseconds(lTime.nanoseconds());
                  lpUser->set_access_count(1);
                  lpUser->set_user_id(lResponse.data.newIdentified.userID);

                  if (!db().create(lpUser))
                  {
                    delete lpUser;
                  }

//                  mbBusyEnrolling = false;
                  mStatus.busy = false;
                }
                else
                {
                  printf("HeimdallCtrl: New enrolled but no active user!\n");
                }
              }
              break;

            case HC_GET_NEW_IDD:
              if (lResponse.data.newIdentified.userID > 0)
              {
                User* lpUser = db().find(lResponse.data.newIdentified.userID);
                if (lpUser)
                {
                  // Update the user's timestamp.
                  Timestamp lTime = Timestamp::Now();
                  lpUser->mutable_last_access()->set_seconds(lTime.seconds());
                  lpUser->mutable_last_access()->set_nanoseconds(lTime.nanoseconds());
                  lpUser->set_access_count(lpUser->access_count() + 1);
                }
              }
              break;

            case HC_REMOVE_ONE:
              if (lResponse.data.removeOne.userID > 0)
              {
                User* lpUser = db().destroy(lResponse.data.removeOne.userID);
                if (lpUser) delete lpUser;
              }
              break;

            case HC_REMOVE_ALL:
              if (lResponse.data.ack.success)
              {
                db().destroyAll();
              }
              break;

            default:
              std::cout << "HeimdallCtrl: Unrecognized command type = "
                        << lResponse.header.type << std::endl;
              break;
            }
          }
        }
      }
    }
    else
    {
      // Attempt to connect to Heimdall.
      if ((Timestamp::Now() - lReconnectTimer).milliseconds() > 100)
      {
//        mbConnected = connect(lSocket, io_service);
        mStatus.connected = connect(lSocket, io_service);
      }
    }
  }

  lSocket.close();
}

//------------------------------------------------------------------------------
bool HeimdallController::
connect(boost::asio::ip::tcp::socket& socket, boost::asio::io_service& service)
{
  bool lbSuccess = false;

  try {
    tcp::resolver resolver(service);
    tcp::resolver::query query(mpHost, boost::lexical_cast<std::string>(mnPort));
    tcp::resolver::iterator iterator = resolver.resolve(query);
    tcp::resolver::iterator end;

    boost::asio::connect(socket, iterator);
    lbSuccess = (iterator != end);
  }
  catch (std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }

  return lbSuccess;
}



