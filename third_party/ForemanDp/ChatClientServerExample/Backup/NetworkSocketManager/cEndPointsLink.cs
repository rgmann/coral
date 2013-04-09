using System;
using System.Collections.Generic;
using System.Text;
using System.Net;

namespace NetworkSocketManager
{
    /// <summary>
    /// This class is to be used with sockets managers as it holds both IPEndPoint's
    /// for a socket, the local and the remote. By using that class it possible to
    /// pass on events with absoulute descrimination between sockets since
    /// there can not be an EndPointsLink similair to another at the same time
    /// anywhere in the network.
    /// </summary>
    public class EndPointsLink:ICloneable
    {
        private IPEndPoint localEndPoint;
        private IPEndPoint remoteEndPoint;

        #region Public Properties

        public IPEndPoint LocalEndPoint
        {
            get { return localEndPoint; }
            set { localEndPoint = value; }
        }

        public IPEndPoint RemoteEndPoint
        {
            get { return remoteEndPoint; }
            set { remoteEndPoint = value; }
        }
        
        #endregion Public Properties

        #region Constructors

        public EndPointsLink(IPEndPoint local, IPEndPoint remote)
        {
            localEndPoint = local;
            remoteEndPoint= remote;
        }

        public EndPointsLink(string endPointsLinkInString)
        {
            try
            {
                string[] sarrEndPoints = endPointsLinkInString.Split(
                    new string[] { " <L-R> " }, StringSplitOptions.None);
                string[] sarrLocalPointData = sarrEndPoints[0].Split(":".ToCharArray());
                string[] sarrRemotePointData = sarrEndPoints[1].Split(":".ToCharArray());
                localEndPoint = new IPEndPoint(
                    IPAddress.Parse(sarrLocalPointData[0]),
                    Convert.ToInt32(sarrLocalPointData[1]));
                remoteEndPoint = new IPEndPoint(
                    IPAddress.Parse(sarrRemotePointData[0]),
                    Convert.ToInt32(sarrRemotePointData[1]));
            }
            catch
            {
                throw new Exception("Parse From String to EndPointsLink - Failed!");
            }
        }

        #endregion Constructors

        #region Overrided public Methods

        public override string ToString()
        {
            string sLocal = "0.0.0.0:#", sRemote = "0.0.0.0:#";
            if (localEndPoint != null) sLocal = localEndPoint.ToString();
            if (remoteEndPoint != null) sRemote = remoteEndPoint.ToString();
            return sLocal + " <L-R> " + sRemote;
        }

        #endregion Overrided public Methods

        #region ICloneable Members

        public object Clone()
        {
            return new EndPointsLink(localEndPoint, remoteEndPoint);
        }

        #endregion
    }
}
