using System;
using System.Collections.Generic;
using System.Text;
using NetworkClient;

namespace NetworkServer
{
    public class NetworkServerClientAccepter : NetworkServerClientWorker,iDataSender
    {
        private Queue<byte[]> dataToSend = new Queue<byte[]>();

        public NetworkServerClientAccepter(TimeSpan requiredLatency, NetworkClientBase clientToAttach)
            : base(requiredLatency, clientToAttach,
            new CommunicationSettings(
            CommunicationSettings.SocketOperationFlow.Synchronic,
            CommunicationSettings.SocketOperationFlow.Synchronic,
            CommunicationSettings.SocketOperationFlow.Synchronic,
            CommunicationSettings.SocketOperationFlow.Synchronic,
            CommunicationSettings.SocketOperationFlow.Synchronic, false, 256))
        {
            //Auto enable this client - so it will not disconnect on first work iteration.
            enable = true;
        }

        public override void AtWorkStart() { }

        public override void AtWorkRoutine()
        {
            //Send & Recieve
            if (dataToSend.Count > 0) client.Send(dataToSend.Dequeue());
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
