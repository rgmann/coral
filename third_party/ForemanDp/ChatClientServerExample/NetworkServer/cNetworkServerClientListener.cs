using System;
using System.Collections.Generic;
using System.Text;
using System.Net;
using NetworkClient;

namespace NetworkServer
{
    public class NetworkServerClientListener : NetworkServerClientWorker
    {
        protected int maxPendingConnections;
        protected IPEndPoint ipeTargetEndpoint;

        public NetworkServerClientListener(TimeSpan requiredLatency, IPEndPoint targetEndPoint,
            int maxPendingConnections)
            : base(targetEndPoint, requiredLatency,
            new CommunicationSettings(
            CommunicationSettings.SocketOperationFlow.Synchronic,
            CommunicationSettings.SocketOperationFlow.Synchronic,
            CommunicationSettings.SocketOperationFlow.Synchronic,
            CommunicationSettings.SocketOperationFlow.Synchronic,
            CommunicationSettings.SocketOperationFlow.Synchronic, false))
        {
            this.maxPendingConnections = maxPendingConnections;
            ipeTargetEndpoint = targetEndPoint;
        }

        public override void AtWorkStart()
        {
            client.Listen(ipeTargetEndpoint, maxPendingConnections);
        }

        public override void AtWorkRoutine()
        {
            client.Accept();
        }

        public override void AtWorkFinish()
        {
            client.Disconnect();
        }

        public override void client_OnClientEvent(NetworkClientBase.ClientEventTypeEnum EventType, object EventData)
        {
            if (EventType == NetworkClientBase.ClientEventTypeEnum.Accepted)
            {
                NetworkServerClientAccepter nsca =
                    new NetworkServerClientAccepter(RequiredLatency, (NetworkClientBase)EventData);
                EventData = nsca;
            }
            base.client_OnClientEvent(EventType, EventData);
        }

    }
}
