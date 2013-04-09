using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using NetworkServer;
using NetworkClient;
using ForemanDP;

namespace ChatServer
{
    public partial class frmMain : Form
    {
        private NetworkServerBase chatServer =
            new NetworkServerBase(new TimeSpan(1000));

        public frmMain()
        {
            InitializeComponent();

            chatServer.OnServerNotify +=
                new NetworkServerBase.DelegateCommunicationServerNotification(chatServer_OnServerNotify);
            
            chatServer.AddListener(
                new NetworkServerClientListener(
                new TimeSpan(2000),
                new System.Net.IPEndPoint(System.Net.IPAddress.Loopback, 3456), 100));
        }

        void chatServer_OnServerNotify(NetworkServerClientWorker worker,
            NetworkServerClientWorker.NetworkClientNotification data)
        {
            switch (((NetworkServerClientWorker.NetworkClientNotification)data).EventType)
            {
                case NetworkClientBase.ClientEventTypeEnum.Accepted:
                    lblConnectedClientsData.BeginInvoke(new dlgUpdater(updateConnectedClientsAmount));
                    break;
                case NetworkClientBase.ClientEventTypeEnum.Disconnected:
                    lblConnectedClientsData.BeginInvoke(new dlgUpdater(updateConnectedClientsAmount));
                    break;
                case NetworkClientBase.ClientEventTypeEnum.MessageRecieved:
                    foreach (NetworkServerClientAccepter accepter in chatServer.Accepters)
                        accepter.DataToSend = (byte[])
                            ((NetworkServerClientWorker.NetworkClientNotification)data).EventData;

                    // This would get a specific worker by key and will send Hello through it.
                    // chatServer.GetAccepter(worker.Key).DataToSend =
                    //     System.Text.Encoding.Unicode.GetBytes("Hello");

                    break;
            }

        }

        private delegate void dlgUpdater();
        private void updateConnectedClientsAmount()
        {
            int i = 0;
            foreach (NetworkServerClientAccepter accepter in chatServer.Accepters) i++;
            lblConnectedClientsData.Text = i.ToString();
        }

        private void btnStart_Click(object sender, EventArgs e)
        {
            btnStart.Enabled = false;
            chatServer.Start();
            btnStop.Enabled = true;
        }

        private void btnStop_Click(object sender, EventArgs e)
        {
            btnStop.Enabled = false;
            chatServer.Stop();
            btnStart.Enabled = true;
        }
    }
}