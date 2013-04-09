using System;
using System.Collections.Generic;
using System.Text;
using System.Net;
using NetworkSocketManager;
using NetworkClient;
using ForemanDP;

namespace NetworkServer
{
    public abstract class NetworkServerClientWorker : BasicWorker
    {
        protected NetworkClientBase client = null;

        public NetworkServerClientWorker(IPEndPoint targetEndPoint, TimeSpan requiredLatency,
            CommunicationSettings workerCommunicationSettings)
            : base(new EndPointsLink(new IPEndPoint(IPAddress.None,0),targetEndPoint).ToString(), requiredLatency)
        {
            client = new NetworkClientBase(workerCommunicationSettings);
            client.OnClientEvent += new NetworkClientBase.DelegateClientEventMethod(client_OnClientEvent);
            enable = true;
        }

        public NetworkServerClientWorker(TimeSpan requiredLatency, NetworkClientBase clientToAttach)
            : base(clientToAttach.ClientEndPointsLink.ToString(), requiredLatency)
        {
            client = clientToAttach;
            client.OnClientEvent += new NetworkClientBase.DelegateClientEventMethod(client_OnClientEvent);
            enable = true;
        }

        public NetworkServerClientWorker(TimeSpan requiredLatency, NetworkClientBase clientToAttach,
            CommunicationSettings workerCommunicationSettings)
            : base(clientToAttach.ClientEndPointsLink.ToString(), requiredLatency)
        {   
            client = clientToAttach;
            client.ClientCommunicationSettings = workerCommunicationSettings;
            client.OnClientEvent += new NetworkClientBase.DelegateClientEventMethod(client_OnClientEvent);
            enable = true;
        }

        public class NetworkClientNotification
        {
            private NetworkClientBase.ClientEventTypeEnum eventType;
            private object eventData;

            
            public NetworkClientNotification(
                NetworkClientBase.ClientEventTypeEnum eventType,object eventData)
            {
                this.eventType = eventType;
                this.eventData = eventData;
            }

            public NetworkClientBase.ClientEventTypeEnum EventType
            {
                get { return eventType; }
            }

            public object EventData
            {
                get { return eventData; }
            }
        }

        public virtual void client_OnClientEvent(NetworkClientBase.ClientEventTypeEnum EventType, object EventData)
        {
            RaiseWorkerNotification(this, new NetworkClientNotification(EventType, EventData));
        }
    }
}
