#ifndef  CHAT_MSG_H
#define  CHAT_MSG_H

#include "BaseTypes.h"

#define  MAX_MSG_LEN 170

struct __attribute__((__packed__)) ChatMsgData
{
   ui32  user_id;
   ui32  timestamp;
   ui32  msg_len;
   char  msg[MAX_MSG_LEN];
};

class ChatMsg
{
public:
   
   ChatMsg();
   
   ChatMsg(const ChatMsg &other);
   
   void reset();
   
   void  setUserId(ui32 userId);
   
   ui32  getUserId() const;
   
   void  setMsg(const char* pMsg, ui32 nMsgLen);
   
   void  getMsg(char* pMsg, ui32 &nMsgLen) const;
   
   void  setTs(ui32 ts);
   
   ui32  getTs() const;
   
   static const ui32 MaxMsgLen = MAX_MSG_LEN;
   
   void pack(ChatMsgData* pMsg) const;
   
   void unpack(const ChatMsgData* pMsg);
   
private:
   
   ui32  m_nUserId;
   
   ui32  m_nTs;
   
   char  m_sMsg[MaxMsgLen];
};

#endif   // CHAT_MSG_H