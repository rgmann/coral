using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using NetworkServer;
using NetworkClient;


namespace MultiClientBenchmark
{
    public partial class frmMain : Form
    {
        private int bm_clients_Connected = 0;
        private int bm_clients_RawDataRecieved = 0;
        private int bm_clients_MessagesRecieved = 0;
        private int bm_clients_MessagesSent = 0;
        private int bm_clients_RemoteDisconnections = 0;
        private int bm_clients_Disconnections = 0;
        private int bm_clients_Errors = 0;

        private int bm_server_Accepted = 0;
        private int bm_server_RawDataRecieved = 0;
        private int bm_server_MessagesRecieved = 0;
        private int bm_server_MessagesSent = 0;
        private int bm_server_RemoteDisconnections = 0;
        private int bm_server_Disconnections = 0;
        private int bm_server_Errors = 0;

        public frmMain()
        {
            InitializeComponent();
        }

        private void btnStartTesting_Click(object sender, EventArgs e)
        {
            btnStartTesting.Enabled = false;

            DateTime StartTime = DateTime.Now;

            #region Setup Varibles

            bm_clients_Connected = 0;
            bm_clients_RawDataRecieved = 0;
            bm_clients_MessagesRecieved = 0;
            bm_clients_MessagesSent = 0;
            bm_clients_RemoteDisconnections = 0;
            bm_clients_Disconnections = 0;
            bm_clients_Errors = 0;

            bm_server_Accepted = 0;
            bm_server_RawDataRecieved = 0;
            bm_server_MessagesRecieved = 0;
            bm_server_MessagesSent = 0;
            bm_server_RemoteDisconnections = 0;
            bm_server_Disconnections = 0;
            bm_server_Errors = 0;

            #endregion Setup Varibles

            #region Get all values into varibles for easy usage

            int clientsAmount = int.Parse(txtbxClientsAmount.Text);
            int packetsToSend = int.Parse(txtbxPacketsToSendPerClient.Text);
            int sizePerPacket = int.Parse(txtbxPacketSizeInBytes.Text);
            int delayBetweenPackets = int.Parse(txtbxDelayBetweenSends.Text);
            int accepterLatency = int.Parse(txtbxAccepterLatency.Text);

            #endregion Get all values into varibles for easy usage.

            #region Gui Setup

            lstbxBMResults_clients.Items.Clear();
            lstbxBMResults_server.Items.Clear();
            for (int iScan = 0; iScan < chkbxTODO.Items.Count; iScan++)
            {
                chkbxTODO.SetSelected(iScan, true);
                chkbxTODO.SetItemCheckState(iScan, CheckState.Unchecked);
            }
            chkbxTODO.ClearSelected();
            pbarTask.Maximum = clientsAmount;
            pbarTask.Value = 0;

            #endregion Gui Setup

            #region Initialize Server & Clients

            chkbxTODO.SetSelected(0, true);
            pbarTask.Value = 0;

            NetworkServerBase benchmarkServer =
                new NetworkServerBase(new TimeSpan(accepterLatency));
            benchmarkServer.OnServerNotify +=
                new NetworkServerBase.DelegateCommunicationServerNotification(
                benchmarkServer_OnServerNotify);
            benchmarkServer.AddListener(
                new NetworkServerClientListener(
                new TimeSpan(1000),
                new System.Net.IPEndPoint(System.Net.IPAddress.Loopback, 3456), 100));
            benchmarkServer.Start();

            List<IdentityClient> clients = new List<IdentityClient>();
            for (int iCount = 0; iCount < clientsAmount; iCount++)
            {
                clients.Add(new IdentityClient());
                clients[iCount].OnClientEvent +=
                    new IdentityClient.DelegateClientEvent(frmMain_OnClientEvent);
                pbarTask.Value = iCount;
                Application.DoEvents();
            }
            chkbxTODO.SetItemCheckState(0, CheckState.Checked);

            #endregion Initialize Server & Clients

            #region Connect All Clients

            chkbxTODO.SetSelected(1, true);
            pbarTask.Value = 0;
            foreach (IdentityClient client in clients)
            {
                client.clientbase.Connect(
                    new System.Net.IPEndPoint(System.Net.IPAddress.Loopback, 3456));
                pbarTask.Value++;
                Application.DoEvents();
            }
            while (
                bm_clients_Connected+
                bm_clients_Disconnections+
                bm_clients_RemoteDisconnections < clientsAmount) Application.DoEvents();
            chkbxTODO.SetItemCheckState(1, CheckState.Checked);

            #endregion Connect All Clients

            #region Sending Packets

            Random rnd = new Random();
            chkbxTODO.SetSelected(2, true);
            pbarTask.Value = 0;
            pbarTask.Maximum = packetsToSend * clientsAmount;
            for (int iScan = 0; iScan < packetsToSend; iScan++)
            {
                foreach (IdentityClient client in clients)
                {
                    client.clientbase.Send(BuildPacket(sizePerPacket, rnd));
                    pbarTask.Value++;
                    Application.DoEvents();
                }
                System.Threading.Thread.Sleep(delayBetweenPackets);
            }
            chkbxTODO.SetItemCheckState(2, CheckState.Checked);

            #endregion Sending Packets

            #region Recieving Packets
            chkbxTODO.SetSelected(3, true);
            int oldErrors = bm_clients_Errors;
            pbarTask.Maximum = packetsToSend * clientsAmount * 2;
            while (
                bm_clients_MessagesSent +
                bm_clients_MessagesRecieved +
                bm_clients_Errors - oldErrors < clientsAmount * packetsToSend * 2)
            {
                pbarTask.Value =
                    bm_clients_MessagesSent +
                    bm_clients_MessagesRecieved + 
                    bm_clients_Errors - oldErrors;
                Application.DoEvents();
            }
            chkbxTODO.SetItemCheckState(3, CheckState.Checked);

            #endregion Recieving Packets

            #region Disconnect All Clients

            chkbxTODO.SetSelected(4, true);
            pbarTask.Maximum = clientsAmount;
            pbarTask.Value = 0;
            foreach (IdentityClient client in clients)
            {
                client.clientbase.Disconnect();
                pbarTask.Value++;
                Application.DoEvents();
            }
            while (
                bm_clients_Disconnections + 
                bm_clients_RemoteDisconnections < clientsAmount) Application.DoEvents();
            chkbxTODO.SetItemCheckState(4, CheckState.Checked);

            #endregion Disconnect All Clients

            TimeSpan tmsp = DateTime.Now.Subtract(StartTime);

            #region Finish

            chkbxTODO.SetSelected(5, true);
            pbarTask.Maximum = 3;
            pbarTask.Value = 0;
            lstbxBMResults_clients.Items.Add("Connected: " +
                bm_clients_Connected.ToString());
            lstbxBMResults_clients.Items.Add("Raw Data Recieved: " +
                bm_clients_RawDataRecieved.ToString());
            lstbxBMResults_clients.Items.Add("Messages Recieved: "+
                bm_clients_MessagesRecieved.ToString());
            lstbxBMResults_clients.Items.Add("Messages Sent: "+
                bm_clients_MessagesSent.ToString());
            lstbxBMResults_clients.Items.Add("Remote Disconnections: " +
                bm_clients_RemoteDisconnections.ToString());
            lstbxBMResults_clients.Items.Add("Disconnections: " +
                bm_clients_Disconnections.ToString());
            lstbxBMResults_clients.Items.Add("Errors: " +
                bm_clients_Errors.ToString());
            pbarTask.Value = 1;
            lstbxBMResults_server.Items.Add("Accepted: " +
                bm_server_Accepted.ToString());
            lstbxBMResults_server.Items.Add("Raw Data Recieved: " +
                bm_server_RawDataRecieved.ToString());
            lstbxBMResults_server.Items.Add("Messages Recieved: " +
                bm_server_MessagesRecieved.ToString());
            lstbxBMResults_server.Items.Add("Messages Sent: " +
                bm_server_MessagesSent.ToString());
            lstbxBMResults_server.Items.Add("Remote Disconnections: " +
                bm_server_RemoteDisconnections.ToString());
            lstbxBMResults_server.Items.Add("Disconnections: " +
                bm_server_Disconnections.ToString());
            lstbxBMResults_server.Items.Add("Errors: " +
                bm_server_Errors.ToString());
            pbarTask.Value = 2;

            lstbxBMResults_clients.Items.Add("-----");
            lstbxBMResults_clients.Items.Add("Total Time: " + tmsp.ToString());

            pbarTask.Value = 3;
            chkbxTODO.SetItemCheckState(5, CheckState.Checked);
            
            #endregion Finish

            btnStartTesting.Enabled = true;
        }

        private byte[] BuildPacket(int packetSize,Random rnd)
        {
            byte[] packet = new byte[packetSize];
            rnd.NextBytes(packet);
            return packet;
        }

        void frmMain_OnClientEvent(NetworkClientBase.ClientEventTypeEnum EventType, object EventData,
            IdentityClient Identity)
        {
            switch (EventType)
            {
                case NetworkClientBase.ClientEventTypeEnum.None:
                    break;
                case NetworkClientBase.ClientEventTypeEnum.Accepted:
                    break;
                case NetworkClientBase.ClientEventTypeEnum.Connected:
                    bm_clients_Connected++;
                    Identity.clientbase.Recieve();
                    break;
                case NetworkClientBase.ClientEventTypeEnum.RawDataRecieved:
                    bm_clients_RawDataRecieved++;
                    break;
                case NetworkClientBase.ClientEventTypeEnum.MessageRecieved:
                    bm_clients_MessagesRecieved++;
                    Identity.clientbase.Recieve();
                    break;
                case NetworkClientBase.ClientEventTypeEnum.MessageSent:
                    bm_clients_MessagesSent++;
                    break;
                case NetworkClientBase.ClientEventTypeEnum.RemoteSideDisconnected:
                    bm_clients_RemoteDisconnections++;
                    break;
                case NetworkClientBase.ClientEventTypeEnum.Disconnected:
                    bm_clients_Disconnections++;
                    break;
                case NetworkClientBase.ClientEventTypeEnum.Error:
                    bm_clients_Errors++;
                    break;
            }
        }
        
        void benchmarkServer_OnServerNotify(NetworkServerClientWorker worker,
            NetworkServerClientWorker.NetworkClientNotification data)
        {
            switch (data.EventType)
            {
                case NetworkClientBase.ClientEventTypeEnum.None:
                    break;
                case NetworkClientBase.ClientEventTypeEnum.Connected:
                    break;
                case NetworkClientBase.ClientEventTypeEnum.Accepted:
                    bm_server_Accepted++;
                    break;
                case NetworkClientBase.ClientEventTypeEnum.RawDataRecieved:
                    bm_server_RawDataRecieved++;
                    break;
                case NetworkClientBase.ClientEventTypeEnum.MessageRecieved:
                    ((NetworkServerClientAccepter)worker).DataToSend = 
                        (byte[])data.EventData;
                    bm_server_MessagesRecieved++;
                    break;
                case NetworkClientBase.ClientEventTypeEnum.MessageSent:
                    bm_server_MessagesSent++;
                    break;
                case NetworkClientBase.ClientEventTypeEnum.RemoteSideDisconnected:
                    bm_server_RemoteDisconnections++;
                    break;
                case NetworkClientBase.ClientEventTypeEnum.Disconnected:
                    bm_server_Disconnections++;
                    break;
                case NetworkClientBase.ClientEventTypeEnum.Error:
                    bm_server_Errors++;
                    break;
            }
        }
    }
}