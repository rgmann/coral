using System;
using System.Collections.Generic;
using System.Text;
using System.Net;
using System.Threading;
using ForemanDP;
using NetworkClient;
using NetworkSocketManager;

namespace NetworkServer
{
    /// <summary>
    /// This is the server base class which is designed to be contained by an application
    /// layer protocol manager.
    /// </summary>
    public class NetworkServerBase
    {
        private BasicServer listeners = new BasicServer();
        private BasicServer accepters = new BasicServer();
        private BasicServer connecters = new BasicServer();

        private TimeSpan newAcceptersLatency = new TimeSpan();

        public delegate void DelegateCommunicationServerNotification(
            NetworkServerClientWorker worker, NetworkServerClientWorker.NetworkClientNotification data);

        public event DelegateCommunicationServerNotification OnServerNotify;

        public NetworkServerBase(TimeSpan newAcceptersLatency)
        {
            this.newAcceptersLatency = newAcceptersLatency;
            listeners.OnServerNotifyEvent +=
                new BasicServer.DelegateServerNotification(BasicServers_OnServerNotifyEvent);
            accepters.OnServerNotifyEvent +=
                new BasicServer.DelegateServerNotification(BasicServers_OnServerNotifyEvent);
            connecters.OnServerNotifyEvent +=
                new BasicServer.DelegateServerNotification(BasicServers_OnServerNotifyEvent);
        }

        public TimeSpan NewAcceptersLatency
        {
            get { return newAcceptersLatency; }
            set { newAcceptersLatency = value; }
        }

        public virtual void Start()
        {
            listeners.Start();
            accepters.Start();
            connecters.Start();
        }

        public virtual void Stop()
        {
            listeners.Stop();
            accepters.Stop();
            connecters.Stop();
        }

        public virtual void Pause()
        {
            listeners.Pause();
            accepters.Pause();
            connecters.Pause();
        }

        public virtual void Continue()
        {
            listeners.Continue();
            accepters.Continue();
            connecters.Continue();
        }

        public void AddListener(NetworkServerClientListener listener)
        {
            listeners.Workers.Add(listener);
        }

        public void RemoveListener(NetworkServerClientListener listener)
        {
            listeners.Workers.Remove(listener);
        }
        
        public NetworkServerClientListener GetListener(string key)
        {
            foreach (NetworkServerClientListener listener in listeners.Workers)
                if (listener.Key == key) return listener;
            return null;
        }

        public void AddConnecter(NetworkServerClientConnecter connecter)
        {
            connecters.Workers.Add(connecter);
        }

        public void RemoveConnecter(NetworkServerClientConnecter connecter)
        {
            connecters.Workers.Remove(connecter);
        }

        public NetworkServerClientConnecter GetConnecter(string key)
        {
            foreach (NetworkServerClientConnecter connecter in connecters.Workers)
                if (connecter.Key == key) return connecter;
            return null;
        }

        public NetworkServerClientAccepter GetAccepter(string key)
        {
            foreach (NetworkServerClientAccepter accepter in accepters.Workers)
                if (accepter.Key == key) return accepter;
            return null;
        }

        public IEnumerable<NetworkServerClientListener> Listeners
        {
            get
            {
                foreach (NetworkServerClientListener listener in listeners.Workers)
                    yield return listener;
            }   
        }

        public IEnumerable<NetworkServerClientConnecter> Connecters
        {
            get
            {
                foreach (NetworkServerClientConnecter connecter in connecters.Workers)
                    yield return connecter;
            }
        }

        public IEnumerable<NetworkServerClientAccepter> Accepters
        {
            get
            {
                foreach (NetworkServerClientAccepter accepter in accepters.Workers)
                    yield return accepter;
            }
        }

        /// <summary>
        /// The listeners, accepters and connecters events are routed to this method.
        /// </summary>
        /// <param name="worker">The worker that raised an event.</param>
        /// <param name="data">relvant event data</param>
        void BasicServers_OnServerNotifyEvent(BasicWorker worker, object data)
        {
            NetworkServerClientWorker.NetworkClientNotification incommingEvent = null;
            if (data.GetType() == typeof(NetworkServerClientWorker.NetworkClientNotification))
            {
                incommingEvent = (NetworkServerClientWorker.NetworkClientNotification)data;
                switch (incommingEvent.EventType)
                {
                    case NetworkClientBase.ClientEventTypeEnum.None:
                        break;
                    case NetworkClientBase.ClientEventTypeEnum.Connected:
                        break;
                    case NetworkClientBase.ClientEventTypeEnum.Accepted:
                        NetworkServerClientAccepter accepter = 
                            (NetworkServerClientAccepter)incommingEvent.EventData;
                        accepter.RequiredLatency = newAcceptersLatency;
                        accepters.Workers.Add(accepter);
                        break;
                    case NetworkClientBase.ClientEventTypeEnum.MessageSent:
                        break;
                    case NetworkClientBase.ClientEventTypeEnum.RawDataRecieved:
                        break;
                    case NetworkClientBase.ClientEventTypeEnum.MessageRecieved:
                        break;
                    case NetworkClientBase.ClientEventTypeEnum.Disconnected:
                        switch (worker.GetType().Name)
                        {
                            case "NetworkServerClientListener": //Controlled by user of this class.
                                break;
                            case "NetworkServerClientAccepter": //This class add it so it should remove it.
                                accepters.Workers.Remove(worker);
                                break;
                            case "NetworkServerClientConnecter"://Controlled by user of this class.
                                break;
                        }
                        break;
                    case NetworkClientBase.ClientEventTypeEnum.Error:
                        break;
                }
            }
            OnServerNotify((NetworkServerClientWorker)worker, incommingEvent);
        }
    }
}