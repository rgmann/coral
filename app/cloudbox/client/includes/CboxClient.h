
class CboxClient
{
public:

private:

   static void monitorThreadEntry(ThreadArg* pArg);
   void monitorThread(ThreadArg* pArg);

   static void txRxThreadEntry(ThreadArg* pArg);
   void txRxThread(ThreadArg* pArg);

private:

   Thread* mpMonitorThread;

   Thread* mpTxRxThread;

};

