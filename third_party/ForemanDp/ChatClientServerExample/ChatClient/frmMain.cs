using System;
using System.Windows.Forms;
using NetworkClient;

namespace ChatClient
{
    public partial class frmMain : Form
    {
        NetworkClientBase chatClient;

        public frmMain()
        {
            InitializeComponent();
        }

        void chatClient_OnClientEvent(NetworkClientBase.ClientEventTypeEnum EventType, object EventData)
        {
            switch (EventType)
            {
                case NetworkClientBase.ClientEventTypeEnum.None:
                    break;
                case NetworkClientBase.ClientEventTypeEnum.Accepted:
                    break;
                case NetworkClientBase.ClientEventTypeEnum.Connected:
                    chatClient.Recieve();
                    break;
                case NetworkClientBase.ClientEventTypeEnum.RawDataRecieved:
                    break;
                case NetworkClientBase.ClientEventTypeEnum.MessageRecieved:
                    txtbxChatData.BeginInvoke(
                        new dlgUpdater(updateChatData),
                        System.Text.Encoding.Unicode.GetString((byte[])EventData));
                    chatClient.Recieve();
                    break;
            }
        }

        private delegate void dlgUpdater(string Data);
        private void updateChatData(string Data)
        {
            txtbxChatData.AppendText(Data);
        }

        private void btnConnect_Click(object sender, EventArgs e)
        {
            btnConnect.Enabled = false;

            chatClient =
                new NetworkClientBase(
                new CommunicationSettings(
                CommunicationSettings.SocketOperationFlow.Asynchronic,
                CommunicationSettings.SocketOperationFlow.Asynchronic,
                CommunicationSettings.SocketOperationFlow.Asynchronic,
                CommunicationSettings.SocketOperationFlow.Asynchronic,
                CommunicationSettings.SocketOperationFlow.Asynchronic, true));

            chatClient.OnClientEvent += chatClient_OnClientEvent;

            chatClient.Connect(
                new System.Net.IPEndPoint(System.Net.IPAddress.Loopback, 3456));

            btnDisconnect.Enabled = true;
        }

        private void btnDisconnect_Click(object sender, EventArgs e)
        {
            btnConnect.Enabled = true;
            chatClient.Disconnect();
            btnDisconnect.Enabled = false;
        }

        private void btnSend_Click(object sender, EventArgs e)
        {
            chatClient.Send(
                System.Text.Encoding.Unicode.GetBytes(txtbxDataToSend.Text));
        }

    }
}