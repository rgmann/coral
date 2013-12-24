

void CboxClient::monitorThreadEntry(ThreadArg* pArg)
{
}

//-----------------------------------------------------------------------------
void CboxClient::monitorThread(ThreadArg* pArg)
{
   while (!pArg->stopSignalled())
   {

      if (!mbConnected) Sleep(1000);

      // Query the server for the next change.
      if (mServerFileMonitor.getChange(lFsChange))
      {
         switch (lFsChange.type)
         {
         }
      }
   }
}

   static void txRxThreadEntry(ThreadArg* pArg);
   void txRxThread(ThreadArg* pArg);

private:

   Thread* mpMonitorThread;

   Thread* mpTxRxThread;

};

