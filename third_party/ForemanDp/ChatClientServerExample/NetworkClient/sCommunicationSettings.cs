using System;
using System.Collections.Generic;
using System.Text;
using System.Reflection;

namespace NetworkClient
{
    public struct CommunicationSettings
    {
        public enum SocketOperationFlow : byte
        {
            Synchronic = 0,
            Asynchronic = 1
        }

        private SocketOperationFlow connect;
        private SocketOperationFlow accept;
        private SocketOperationFlow send;
        private SocketOperationFlow recieve;
        private SocketOperationFlow disconnect;

        private int asyncRecieveBufferSize;
        private bool syncUsesBlockingMode;

        #region Public Properties

        public SocketOperationFlow Connect
        {
            get { return connect; }
            set { connect = value; }
        }

        public SocketOperationFlow Accept
        {
            get { return accept; }
            set { accept = value; }
        }

        public SocketOperationFlow Send
        {
            get { return send; }
            set { send = value; }
        }

        public SocketOperationFlow Recieve
        {
            get { return recieve; }
            set { recieve = value; }
        }

        public SocketOperationFlow Disconnect
        {
            get { return disconnect; }
            set { disconnect = value; }
        }

        /// <summary>
        /// The recieve buffer size in the asynchronic flow (default is 256).
        /// </summary>
        public int AsyncRecieveBufferSize
        {
            get { return asyncRecieveBufferSize; }
            set { asyncRecieveBufferSize = value; }
        }

        /// <summary>
        /// Determine if synchronous operation will block the socket or not.
        /// </summary>
        public bool SyncUsesBlockingMode
        {
            get { return syncUsesBlockingMode; }
            set { syncUsesBlockingMode = value; }
        }

        #endregion Public Properties

        #region Constructors

        public CommunicationSettings(SocketOperationFlow connect,
            SocketOperationFlow accept, SocketOperationFlow send,
            SocketOperationFlow recieve, SocketOperationFlow disconnect,
            bool syncUsesBlockingMode)
        {
            this.connect = connect;
            this.accept = accept;
            this.send = send;
            this.recieve = recieve;
            this.disconnect = disconnect;
            this.syncUsesBlockingMode = syncUsesBlockingMode;
            this.asyncRecieveBufferSize = 256;
        }

        public CommunicationSettings(SocketOperationFlow connect,
            SocketOperationFlow accept, SocketOperationFlow send,
            SocketOperationFlow recieve, SocketOperationFlow disconnect,
            bool syncUsesBlockingMode, int asyncRecieveBufferSize)
        {
            this.connect = connect;
            this.accept = accept;
            this.send = send;
            this.recieve = recieve;
            this.disconnect = disconnect;
            this.syncUsesBlockingMode = syncUsesBlockingMode;
            this.asyncRecieveBufferSize = asyncRecieveBufferSize;
        }

        #endregion Constructors
    }
}
