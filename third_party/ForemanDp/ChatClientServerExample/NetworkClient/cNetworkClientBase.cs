using System;
using System.Collections.Generic;
using System.Text;
using System.Net;
using NetworkSocketManager;
//using ByteArrayOperations;

namespace NetworkClient
{
    /// <summary>
    /// This class encapsulate a client which uses a single socket for communication.
    /// The communication role can be either a listener or a connecter.
    /// Using this class it is possible to define the opearation methodology for each 
    /// command opeartion using the CommunicationSettings structure.
    /// This class can be overrided to set a 2nd layer for transmition control.
    /// </summary>
    public class NetworkClientBase
    {
        public enum NetworkClientMode:byte
        {
            None=0,
            Connecter=1,
            Listener=2
        }

        private NetworkSocketManagerTCP skt;
        private CommunicationSettings clientCommunicationSettings;
        private NetworkClientMode clientMode;

        private byte[] incommingData=new byte[0];

        public enum ClientEventTypeEnum : byte
        {
            None = 0,
            Connected = 1,
            Accepted = 2,
            MessageSent = 3,
            RawDataRecieved = 4,
            MessageRecieved = 5,
            Disconnected = 6,
            RemoteSideDisconnected = 7,
            Error = 8
        }

        public delegate void DelegateClientEventMethod(ClientEventTypeEnum EventType, object EventData);

        /// <summary>
        /// All client events will channel through this event.
        /// </summary>
        public event DelegateClientEventMethod OnClientEvent;

        #region Public Properties

        public CommunicationSettings ClientCommunicationSettings
        {
            get { return clientCommunicationSettings; }
            set { clientCommunicationSettings = value; }
        }

        public NetworkClientMode ClientMode
        {
            get { return clientMode; }
        }

        public EndPointsLink ClientEndPointsLink
        {
            get { return skt.SocketEndPointsLink; }
        }

        #endregion Public Properties

        #region Constructors

        /// <summary>
        /// Open a new client with default settings (everything beside accept is set to asynchronic).
        /// </summary>
        public NetworkClientBase(CommunicationSettings clientCommunicationSettings)
        {
            clientMode = NetworkClientMode.None;
            this.clientCommunicationSettings = clientCommunicationSettings;
            skt = new NetworkSocketManagerTCP();
            skt.OnSocketEvent += new NetworkSocketManagerTCP.DelegateSocketEventMethod(skt_OnSocketEvent);
        }

        /// <summary>
        /// Open a new client by attaching an existing socket.
        /// </summary>
        /// <param name="socketManagerTCP">The socket to attach.</param>
        public NetworkClientBase(NetworkSocketManagerTCP socketManagerTCP)
        {
            clientMode = NetworkClientMode.None;
            skt = socketManagerTCP;
            skt.OnSocketEvent += new NetworkSocketManagerTCP.DelegateSocketEventMethod(skt_OnSocketEvent);
        }

        /// <summary>
        /// Open a new client by attaching an existing socket. (with the option to alter the client settings)
        /// </summary>
        /// <param name="socketManagerTCP">The socket to attach.</param>
        /// <param name="clientCommunicationSettings">Settings to alter.</param>
        public NetworkClientBase(NetworkSocketManagerTCP socketManagerTCP,
            CommunicationSettings clientCommunicationSettings)
        {
            clientMode = NetworkClientMode.None;
            this.clientCommunicationSettings = clientCommunicationSettings;
            skt = socketManagerTCP;
            skt.OnSocketEvent += new NetworkSocketManagerTCP.DelegateSocketEventMethod(skt_OnSocketEvent);
        }

        #endregion Constructors

        #region Public Methods

        /// <summary>
        /// Connect using the settings given through ClientCommunicationSettings.
        /// </summary>
        public void Connect(IPEndPoint remoteTarget)
        {
            clientMode = NetworkClientMode.Connecter;
            skt.SocketEndPointsLink.RemoteEndPoint = remoteTarget;
            skt.BlockingMode = clientCommunicationSettings.SyncUsesBlockingMode;
            switch (clientCommunicationSettings.Connect)
            {
                case CommunicationSettings.SocketOperationFlow.Synchronic:
                    skt.Sync_Connect();
                    break;
                case CommunicationSettings.SocketOperationFlow.Asynchronic:
                    skt.Async_Connect();
                    break;
            }
        }

        /// <summary>
        /// Accept using the settings given through ClientCommunicationSettings.
        /// </summary>
        public void Accept()
        {
            switch (clientCommunicationSettings.Accept)
            {
                case CommunicationSettings.SocketOperationFlow.Synchronic:
                    skt.Sync_Accept();
                    break;
                case CommunicationSettings.SocketOperationFlow.Asynchronic:
                    skt.Async_Accept();
                    break;
            }
        }

        /// <summary>
        /// Send using the settings given through ClientCommunicationSettings.
        /// </summary>
        /// <param name="buffer">Data to send.</param>
        public void Send(byte[] buffer)
        {
            buffer=ApplicationLayerTransmitionControl.AddPrefix(buffer);
            switch (clientCommunicationSettings.Send)
            {
                case CommunicationSettings.SocketOperationFlow.Synchronic:
                    skt.Sync_Send(buffer);
                    break;
                case CommunicationSettings.SocketOperationFlow.Asynchronic:
                    skt.Async_Send(buffer);
                    break;
            }
        }

        /// <summary>
        /// Recieve using the settings given through ClientCommunicationSettings.
        /// </summary>
        public void Recieve()
        {
            switch (clientCommunicationSettings.Recieve)
            {
                case CommunicationSettings.SocketOperationFlow.Synchronic:
                    skt.Sync_Recieve();
                    break;
                case CommunicationSettings.SocketOperationFlow.Asynchronic:
                    skt.Async_Recieve(clientCommunicationSettings.AsyncRecieveBufferSize);
                    break;
            }
        }

        /// <summary>
        /// Disconnect using the settings given through ClientCommunicationSettings.
        /// </summary>
        public void Disconnect()
        {
            switch (clientCommunicationSettings.Disconnect)
            {
                case CommunicationSettings.SocketOperationFlow.Synchronic:
                    skt.Sync_Disconnect();
                    break;
                case CommunicationSettings.SocketOperationFlow.Asynchronic:
                    skt.Async_Disconnect();
                    break;
            }
        }

        /// <summary>
        /// Starts listening to incomming connections, use disconnect to finish listening.
        /// </summary>
        /// <param name="localTarget"> Where to listen to incomming connections.</param>
        /// <param name="maxPendingConnections">Amount of pending connection in case of congestion.</param>
        public void Listen(IPEndPoint localTarget, int maxPendingConnections)
        {
            clientMode = NetworkClientMode.Listener;
            skt.SocketEndPointsLink.LocalEndPoint = localTarget;
            skt.Listen(maxPendingConnections);
            skt.BlockingMode = clientCommunicationSettings.SyncUsesBlockingMode;
        }        

        #endregion Public Methods

        private void skt_OnSocketEvent(NetworkSocketManagerTCP.SocketEventTypeEnum EventType, object EventData)
        {
            switch (EventType)
            {
                case NetworkSocketManagerTCP.SocketEventTypeEnum.None:
                    if (OnClientEvent != null)
                        OnClientEvent.BeginInvoke(ClientEventTypeEnum.None, EventData, null, null);
                    break;
                case NetworkSocketManagerTCP.SocketEventTypeEnum.Connected:
                    if (OnClientEvent != null)
                        OnClientEvent.BeginInvoke(ClientEventTypeEnum.Connected, EventData, null, null);
                    break;
                case NetworkSocketManagerTCP.SocketEventTypeEnum.Accepted:
                    if (OnClientEvent != null)
                    {
                        NetworkClientBase nwcbAccepter =
                            new NetworkClientBase((NetworkSocketManagerTCP)EventData);
                        OnClientEvent.BeginInvoke(ClientEventTypeEnum.Accepted, nwcbAccepter, null, null);
                    }
                    break;
                case NetworkSocketManagerTCP.SocketEventTypeEnum.DataSent:
                    if (OnClientEvent != null)
                        OnClientEvent.BeginInvoke(ClientEventTypeEnum.MessageSent, EventData, null, null);
                    break;
                case NetworkSocketManagerTCP.SocketEventTypeEnum.DataRecieved:
                    if (((byte[])EventData).Length > 0)
                    {
                        if (OnClientEvent != null)
                            OnClientEvent.BeginInvoke(ClientEventTypeEnum.RawDataRecieved,
                               ((byte[])EventData), null, null);

                        incommingData = ByteArrayOperations.AddByteArrays(incommingData, (byte[])EventData);
                        byte[] data = null;
                        while (ApplicationLayerTransmitionControl.IsIntegratedDataWaiting(incommingData))
                        {
                            data = ApplicationLayerTransmitionControl.
                                ExtractIntegratedDataFromBuffer(ref incommingData);
                            if (OnClientEvent != null)
                                OnClientEvent.BeginInvoke(ClientEventTypeEnum.MessageRecieved,
                                          data, null, null);
                        }
                        if (incommingData.Length > 0) Recieve();
                    }
                    else if (OnClientEvent != null)
                        OnClientEvent.BeginInvoke(ClientEventTypeEnum.Disconnected, EventData, null, null);
                    break;
                case NetworkSocketManagerTCP.SocketEventTypeEnum.Disconnected:
                    skt.Dispose();
                    if (OnClientEvent != null)
                        OnClientEvent.BeginInvoke(ClientEventTypeEnum.Disconnected, EventData, null, null);
                    break;
                case NetworkSocketManagerTCP.SocketEventTypeEnum.RemoteSideDisconnected:
                    Disconnect();
                    if (OnClientEvent != null)
                        OnClientEvent.BeginInvoke(ClientEventTypeEnum.RemoteSideDisconnected, EventData, null, null);
                    break;
                case NetworkSocketManagerTCP.SocketEventTypeEnum.Error:
                    if (OnClientEvent != null)
                        OnClientEvent.BeginInvoke(ClientEventTypeEnum.Error, EventData, null, null);
                    break;
            }
        }
    }
}
