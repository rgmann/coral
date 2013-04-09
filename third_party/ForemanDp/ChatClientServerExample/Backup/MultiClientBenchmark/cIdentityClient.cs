using System;
using System.Collections.Generic;
using System.Text;
using NetworkClient;

namespace MultiClientBenchmark
{
    public class IdentityClient
    {
        public NetworkClientBase clientbase;

        public delegate void DelegateClientEvent(
            NetworkClientBase.ClientEventTypeEnum EventType,
            object EventData,
            IdentityClient Identity);

        public event DelegateClientEvent OnClientEvent;

        private int packetsSent = 0;

        public int PacketsSent
        {
            get { return packetsSent; }
        }

        private bool isConnected = false;

        public bool IsConnected
        {
            get { return isConnected; }
        }

        public Guid guid = Guid.NewGuid();

        public IdentityClient()
        {
            clientbase = new NetworkClientBase(
                new CommunicationSettings(
                CommunicationSettings.SocketOperationFlow.Asynchronic,
                CommunicationSettings.SocketOperationFlow.Asynchronic,
                CommunicationSettings.SocketOperationFlow.Asynchronic,
                CommunicationSettings.SocketOperationFlow.Asynchronic,
                CommunicationSettings.SocketOperationFlow.Asynchronic, true));
            clientbase.OnClientEvent += 
                new NetworkClientBase.DelegateClientEventMethod(clientbase_OnClientEvent);
        }

        //object locker = new object();
        void clientbase_OnClientEvent(NetworkClientBase.ClientEventTypeEnum EventType, object EventData)
        {
            //lock (locker)
            //{
                switch (EventType)
                {
                    case NetworkClientBase.ClientEventTypeEnum.Connected:
                        isConnected = true;
                        break;
                    case NetworkClientBase.ClientEventTypeEnum.MessageRecieved:
                        break;
                    case NetworkClientBase.ClientEventTypeEnum.MessageSent:
                        packetsSent++;
                        break;
                    case NetworkClientBase.ClientEventTypeEnum.Disconnected:
                        isConnected = false;
                        break;
                    case NetworkClientBase.ClientEventTypeEnum.RemoteSideDisconnected:
                        break;
                    case NetworkClientBase.ClientEventTypeEnum.Error:
                        break;
                }
            //}
            if (OnClientEvent != null) OnClientEvent(EventType, EventData, this);
        }
    }
}
