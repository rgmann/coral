using System;
using System.Collections.Generic;
using System.Text;
using System.Net;
using System.Net.Sockets;
using System.Threading;

namespace NetworkSocketManager
{
    /// <summary>
    /// This class is a wrapping class for particular type of socket, The TCP socket;
    /// Which is being used as standard in the Internet. Using this class it is possible
    /// to manage the socket activity as required by setting a work methodology for each
    /// socket operation. This class holds those settings and operates the TCP socket.
    /// Each socket operation can be synchornic or asynchronic except for listen which is
    /// always synchronic.
    /// Also note that a socket in listening mode have no remote IPEndPoint.
    /// The class also provide an event that other class can register to in order to
    /// acknowledge an operation occurance.
    /// </summary>
    public class NetworkSocketManagerTCP:IDisposable
    {
        private Socket skt;
        private EndPointsLink socketEndPointsLink;
        private bool listenMode = false;

        private byte[] RecieveBuffer; //Used with Async_Recieve.

        //Those varibles will be used to make sure a correct disposal is being made.
        private IAsyncResult PendingConnect = null;
        private IAsyncResult PendingAccept = null;
        private IAsyncResult PendingSend = null;
        private IAsyncResult PendingRecieve = null;
        private IAsyncResult PendingDisconnect = null;

        private object oLocker = new object();

        public enum SocketEventTypeEnum : byte
        {
            None = 0,
            Connected = 1,
            Accepted = 2,
            DataSent = 3,
            DataRecieved = 4,
            Disconnected = 5,
            RemoteSideDisconnected = 6,
            Error = 7
        }

        public delegate void DelegateSocketEventMethod(SocketEventTypeEnum EventType, object EventData);

        /// <summary>
        /// All socket events will channel through this event.
        /// </summary>
        public event DelegateSocketEventMethod OnSocketEvent;

        #region Public Properties
        
        public EndPointsLink SocketEndPointsLink
        {
            get { return socketEndPointsLink; }
            set { socketEndPointsLink = value; }
        }

        public bool ListenMode
        {
            get { return listenMode; }
        }

        /// <summary>
        /// Determines if the synchronous operations will block execution or not.
        /// </summary>
        public bool BlockingMode
        {
            get { return skt.Blocking; }
            set { skt.Blocking = value; }
        }

        #endregion Public Properties

        #region Constructors

        /// <summary>
        /// General constructor to be used when no prior information is availble for the socket.
        /// </summary>
        public NetworkSocketManagerTCP()
        {
            skt = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
            socketEndPointsLink = new EndPointsLink(
                (IPEndPoint)skt.LocalEndPoint, new IPEndPoint(IPAddress.None,0));
        }

        /// <summary>
        /// A constructor to specify the target IPEndPoint.
        /// Use this constructor for a listening socket or a connecting socket.
        /// </summary>
        /// <param name="targetIPEndPoint">The target IPEndPoint to listen for or to connect to.</param>
        public NetworkSocketManagerTCP(IPEndPoint targetIPEndPoint)
        {
            skt = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
            socketEndPointsLink = new EndPointsLink(
                (IPEndPoint)skt.LocalEndPoint, targetIPEndPoint);
            listenMode = false;
        }

        /// <summary>
        /// A constructor to attach a socket.
        /// Use this constructor when accepting sockets.
        /// </summary>
        /// <param name="sktTarget">The socket to attach.</param>
        public NetworkSocketManagerTCP(Socket sktTarget)
        {
            skt = sktTarget;
            socketEndPointsLink = new EndPointsLink(
                (IPEndPoint)skt.LocalEndPoint, (IPEndPoint)skt.RemoteEndPoint);
            listenMode = false;
        }

        #endregion Constructors

        #region Public Methods

        /// <summary>
        /// Instruct the socket to start listening for incomming connections.
        /// Note: to finish listening use disconnect method.
        /// </summary>
        /// <param name="maxPendingConnections">Maxmimum amount of connections that can wait while the
        /// listener is busy.</param>
        public void Listen(int maxPendingConnections)
        {
            bool bError = false;
            try
            {
                skt.Bind(socketEndPointsLink.LocalEndPoint);
                skt.Listen(maxPendingConnections);
                listenMode = true;
            }
            catch { bError = true; }
            if (OnSocketEvent != null && bError)
                OnSocketEvent(SocketEventTypeEnum.Error, "Socket already listening.");
        }

        #region Connect

        /// <summary>
        /// Connect synchronic.
        /// </summary>
        public void Sync_Connect()
        {
            bool bError = false;
            try
            {
                skt.Connect(socketEndPointsLink.RemoteEndPoint);
            }
            catch //(Exception e)
            {
                bError = true; 
            }
            if (OnSocketEvent != null)
            {
                if (bError)
                    OnSocketEvent.BeginInvoke(SocketEventTypeEnum.Error, "Sync_Connect Failed.", null, null);
                else
                    OnSocketEvent.BeginInvoke(SocketEventTypeEnum.Connected, null, null, null);
            }
        }

        /// <summary>
        /// Connect asynchronic.
        /// </summary>
        public void Async_Connect()
        {
            //lock (oLocker)
            {
                bool bError = false;
                try
                {
                    //while (PendingConnect != null && !PendingConnect.IsCompleted) Thread.Sleep(0);
                    PendingConnect = skt.BeginConnect(socketEndPointsLink.RemoteEndPoint, new AsyncCallback(Async_Connect_Callback), null);
                }
                catch { bError = true; }
                if (OnSocketEvent != null && bError)
                    OnSocketEvent.BeginInvoke(SocketEventTypeEnum.Error, "Async_Connect Failed.", null, null);
            }
        }

        private void Async_Connect_Callback(IAsyncResult ar)
        {
            bool bError = false;
            try
            {
                skt.EndConnect(ar);
            }
            catch { bError = true; }
            if (OnSocketEvent != null)
            {
                if (bError)
                    OnSocketEvent.BeginInvoke(SocketEventTypeEnum.Error, "Async_Connect_Callback Failed.", null, null);
                else
                    OnSocketEvent.BeginInvoke(SocketEventTypeEnum.Connected, null, null, null);
            }
        }

        #endregion Connect

        #region Accept

        /// <summary>
        /// Accept synchronic.
        /// </summary>
        public void Sync_Accept()
        {
            bool bError = false;
            bool bAcceptInvoked = false;
            NetworkSocketManagerTCP acceptingSocket = null;
            try
            {
                if (BlockingMode)
                {
                    bAcceptInvoked = true;
                    Socket sktAccepted = skt.Accept();
                    acceptingSocket = new NetworkSocketManagerTCP(sktAccepted);
                }
                else //not blocking mode, check if a socket is pending.
                    if (skt.Poll(1000, SelectMode.SelectRead))
                    {
                        bAcceptInvoked = true;
                        Socket sktAccepted = skt.Accept();
                        acceptingSocket = new NetworkSocketManagerTCP(sktAccepted);
                    }
            }
            catch { bError = true; }
            if (OnSocketEvent != null)
            {
                if (bError)
                    OnSocketEvent.BeginInvoke(SocketEventTypeEnum.Error, "Sync_Accept Failed.", null, null);
                else
                    if(bAcceptInvoked) 
                        OnSocketEvent.BeginInvoke(SocketEventTypeEnum.Accepted, acceptingSocket, null, null);
            }
        }

        /// <summary>
        /// Accept asynchronic.
        /// </summary>
        public void Async_Accept()
        {
            //lock (oLocker)
            {
                bool bError = false;
                try
                {
                    //while (PendingAccept != null && !PendingAccept.IsCompleted) Thread.Sleep(0);
                    PendingAccept = skt.BeginAccept(new AsyncCallback(Async_Accept_Callback), null);
                }
                catch { bError = true; }
                if (OnSocketEvent != null && bError)
                    OnSocketEvent.BeginInvoke(SocketEventTypeEnum.Error, "Async_Accept Failed.", null, null);
            }
        }

        private void Async_Accept_Callback(IAsyncResult ar)
        {
            bool bError = false;
            NetworkSocketManagerTCP AcceptingSocket = null;
            try
            {
                Socket sktAccepted = skt.EndAccept(ar);
                AcceptingSocket = new NetworkSocketManagerTCP(sktAccepted);
            }
            catch { bError = true; }
            if (OnSocketEvent != null)
            {
                if (bError)
                    OnSocketEvent.BeginInvoke(SocketEventTypeEnum.Error, "Async_Accept_Callback Failed.", null, null);
                else
                    OnSocketEvent.BeginInvoke(SocketEventTypeEnum.Accepted, AcceptingSocket, null, null);
            }
        }

        #endregion Accept

        #region Send

        /// <summary>
        /// Send synchronic.
        /// </summary>
        /// <param name="buffer">Data to send.</param>
        public void Sync_Send(byte[] buffer)
        {
            bool bError = false;
            try
            {
                int sentAmount = 0;
                int portionSent = 0;
                while (sentAmount < buffer.Length && !bError)
                {
                    if (skt.Poll(100, SelectMode.SelectWrite))
                    {
                        portionSent = skt.Send(buffer);
                        sentAmount += portionSent;
                        buffer = ByteArrayOperations.RemoveFromByteArray(buffer, 0, portionSent);
                    }
                    else bError = skt.Poll(100, SelectMode.SelectError);
                }
            }
            catch
            {
                bError = true;
            }
            if (OnSocketEvent != null)
            {
                if (bError)
                    OnSocketEvent.BeginInvoke(SocketEventTypeEnum.Error, "Sync_Send Failed.", null, null);
                else
                    OnSocketEvent.BeginInvoke(SocketEventTypeEnum.DataSent, buffer, null, null);
            }            
        }
        
        /// <summary>
        /// Send asynchronic.
        /// </summary>
        /// <param name="buffer">Data to send.</param>
        public void Async_Send(byte[] buffer)
        {
            bool bError = false;
            try
            {
                SocketError sktErr;
                if (skt.Poll(100, SelectMode.SelectWrite) &&
                    !skt.Poll(100, SelectMode.SelectError))
                {
                    PendingSend = skt.BeginSend(
                        buffer, 0, buffer.Length, SocketFlags.None, out sktErr,
                        new AsyncCallback(Async_Send_Callback), buffer);
                    if (sktErr != SocketError.Success) bError = true;
                }
                else bError = true;
            }
            catch
            {
                bError = true;
            }
            if (OnSocketEvent != null && bError)
                OnSocketEvent.BeginInvoke(SocketEventTypeEnum.Error, "Async_Send Failed.", null, null);
        }

        private void Async_Send_Callback(IAsyncResult ar)
        {
            bool bError = false;
            try
            {
                SocketError sktErr;
                byte[] buffer =(byte[])ar.AsyncState;
                int iSent=skt.EndSend(ar, out sktErr);
                if (iSent < buffer.Length) bError = true;
                if (sktErr != SocketError.Success) bError = true;
            }
            catch
            {
                bError = true; 
            }
            if (OnSocketEvent != null)
            {
                if (bError)
                    OnSocketEvent.BeginInvoke(SocketEventTypeEnum.Error, "Async_Send_Callback Failed.", null, null);
                else
                    OnSocketEvent.BeginInvoke(SocketEventTypeEnum.DataSent, ar.AsyncState, null, null);
            }
        }

        #endregion Send

        #region Recieve

        /// <summary>
        /// Recieve synchronic.
        /// </summary>
        public void Sync_Recieve()
        {
            bool bError = false;
            byte[] buffer = new byte[0];
            bool bDisconnected =
                !skt.Poll(100, SelectMode.SelectRead) &&
                !skt.Poll(100, SelectMode.SelectWrite) &&
                !skt.Poll(100, SelectMode.SelectError);
            if(!bDisconnected)
            {
                while (skt.Connected && skt.Available > 0)
                {
                    if (skt.Poll(100, SelectMode.SelectRead) &&
                        !skt.Poll(100, SelectMode.SelectError))
                    {
                        byte[] portion = new byte[skt.Available];
                        try
                        {
                            skt.Receive(portion);
                        }
                        catch
                        {
                            
                        }
                        buffer = ByteArrayOperations.AddByteArrays(buffer, portion);
                    }
                    else bError = true;
                }
                bDisconnected = !skt.Connected;
            }
            if (OnSocketEvent != null)
            {
                if (bError)
                    OnSocketEvent.BeginInvoke(
                        SocketEventTypeEnum.Error, null, null, null);
                if (bDisconnected)
                    OnSocketEvent.BeginInvoke(
                        SocketEventTypeEnum.RemoteSideDisconnected, null, null, null);
                if (buffer.Length > 0)
                    OnSocketEvent.BeginInvoke(
                        SocketEventTypeEnum.DataRecieved, buffer, null, null);
            }
        }

        /// <summary>
        /// Recieve asynchronic.
        /// </summary>
        public void Async_Recieve(int recieveSizeInBytes)
        {
            bool bError = false;
            if (PendingRecieve == null || PendingRecieve.IsCompleted)
            {
                try
                {
                    SocketError sktErr;
                    RecieveBuffer = new byte[recieveSizeInBytes];
                    PendingRecieve = skt.BeginReceive(RecieveBuffer, 0, RecieveBuffer.Length,
                        SocketFlags.None, out sktErr, new AsyncCallback(Async_Recieve_Callback), null);
                    if (sktErr != SocketError.Success) bError = true;
                }
                catch
                {
                    bError = true;
                }
                if (OnSocketEvent != null && bError)
                    OnSocketEvent.BeginInvoke(SocketEventTypeEnum.Error, "Async_Recieve Failed.", null, null);
            }
        }

        private void Async_Recieve_Callback(IAsyncResult ar)
        {
            bool bDisconnected = false;
            bool bError = false;
            int iAmountRecieved = 0;
            try
            {
                SocketError sktErr = SocketError.Success;
                if (skt.Connected)
                {
                    iAmountRecieved = skt.EndReceive(ar, out sktErr);
                }
                else
                {
                    bDisconnected = true;
                }
                if (sktErr != SocketError.Success) bError = true;
            }
            catch
            {
                bError = true;
            }
            if (OnSocketEvent != null)
            {
                if (bError)
                    OnSocketEvent.BeginInvoke(
                        SocketEventTypeEnum.Error, "Async_Recieve_Callback Failed.", null, null);
                if (bDisconnected)
                    OnSocketEvent.BeginInvoke(
                        SocketEventTypeEnum.RemoteSideDisconnected, null, null, null);
                if (iAmountRecieved > 0)
                {
                    byte[] ExactAmountOfRecievedDataBuffer = new byte[iAmountRecieved];
                    for (int iScan = 0; iScan < iAmountRecieved; iScan++)
                        ExactAmountOfRecievedDataBuffer[iScan] = RecieveBuffer[iScan];
                    OnSocketEvent.BeginInvoke(SocketEventTypeEnum.DataRecieved, ExactAmountOfRecievedDataBuffer, null, null);
                }
            }
        }

        #endregion Recieve

        #region Disconnect

        /// <summary>
        /// Disconnect synchronic.
        /// </summary>
        public void Sync_Disconnect()
        {
            bool bError = false;
            try
            {
                if (skt.Connected) skt.Disconnect(false);
            }
            catch
            {
                bError = true;
            }
            if (OnSocketEvent != null)
            {
                if (bError)
                    OnSocketEvent.BeginInvoke(SocketEventTypeEnum.Error, "Sync_Disconnect Failed.", null, null);
                else
                    OnSocketEvent.BeginInvoke(SocketEventTypeEnum.Disconnected, null, null, null);
            }
        }

        /// <summary>
        /// Disconnect asynchronic.
        /// </summary>
        public void Async_Disconnect()
        {
            bool bError = false;
            try
            {
                if (skt.Connected)
                {
                    PendingDisconnect = skt.BeginDisconnect(
                        false, new AsyncCallback(Async_Disconnect_Callback), null);
                }
            }
            catch
            {
                bError = true;
            }
            if (OnSocketEvent != null && bError)
                OnSocketEvent.BeginInvoke(SocketEventTypeEnum.Error, "Async_Disconnect Failed.", null, null);
        }

        private void Async_Disconnect_Callback(IAsyncResult ar)
        {
            bool bError = false;
            try
            {
                skt.EndDisconnect(ar);
            }
            catch
            {
                bError = true; 
            }
            if (OnSocketEvent != null)
            {
                if (bError)
                    OnSocketEvent.BeginInvoke(
                        SocketEventTypeEnum.Error, "Async_Disconnect_Callback Failed.", null, null);
                else
                    OnSocketEvent.BeginInvoke(SocketEventTypeEnum.Disconnected, null, null, null);
            }
        }

        #endregion Disconnect

        #endregion Public Methods

        #region IDisposable Members

        public void Dispose()
        {
            if (listenMode || skt.Connected)
                if (PendingDisconnect == null)
                {
                    try { skt.Disconnect(true); }
                    catch 
                    {
                        OnSocketEvent.BeginInvoke(SocketEventTypeEnum.Error,
                            "Disconnecting Failed From Dispose Procedure in cNetworkSocketManagerTCP.",
                            null, null);
                    }
                }
            if (skt.Connected) skt.Shutdown(SocketShutdown.Both);
            skt.Close();
            bool bOkToDispose = true;
            do
            {
                bOkToDispose = true;
                if (PendingConnect != null) if (!PendingConnect.IsCompleted) bOkToDispose = false;
                if (PendingAccept != null) if (!PendingAccept.IsCompleted) bOkToDispose = false;
                if (PendingSend != null) if (!PendingSend.IsCompleted) bOkToDispose = false;
                if (PendingRecieve != null) if (!PendingRecieve.IsCompleted) bOkToDispose = false;
                if (PendingDisconnect != null) if (!PendingDisconnect.IsCompleted) bOkToDispose = false;
                if(!bOkToDispose) Thread.Sleep(100); //wait 100 milisecs before next iteration.
            }
            while (!bOkToDispose);
        }

        #endregion
    }
}
