using System;
using System.Collections.Generic;
using System.Text;
using System.Net;
using NetworkClient;

namespace NetworkServer
{
    public class NetworkServerClientConnecter : NetworkServerClientWorker,iDataSender
    {
        private IPEndPoint ipeTargetEndpoint;

        private Queue<byte[]> dataToSend = new Queue<byte[]>();

        public NetworkServerClientConnecter(TimeSpan requiredLatency, IPEndPoint targetEndPoint)
            : base(targetEndPoint , requiredLatency, new CommunicationSettings(
            CommunicationSettings.SocketOperationFlow.Synchronic,
            CommunicationSettings.SocketOperationFlow.Synchronic,
            CommunicationSettings.SocketOperationFlow.Synchronic,
            CommunicationSettings.SocketOperationFlow.Synchronic,
            CommunicationSettings.SocketOperationFlow.Synchronic, false, 256))
        {
            ipeTargetEndpoint = targetEndPoint;
        }

        public override void AtWorkStart()
        {
            client.Connect(ipeTargetEndpoint);
        }

        public override void AtWorkRoutine()
        {
            //TODO: we need to know it was accepted in the other side before - need to check that running.
            //Send & Recieve
            if (dataToSend.Count>0) client.Send(dataToSend.Dequeue());
            client.Recieve();
        }

        public override void AtWorkFinish()
        {
            client.Disconnect();
        }

        #region iDataSender Members

        public byte[] DataToSend
        {
            set
            {
                dataToSend.Enqueue(value);
            }
        }

        #endregion
    }
}
