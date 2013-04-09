namespace MultiClientBenchmark
{
    partial class frmMain
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.btnStartTesting = new System.Windows.Forms.Button();
            this.lblClientsAmount = new System.Windows.Forms.Label();
            this.txtbxClientsAmount = new System.Windows.Forms.TextBox();
            this.txtbxPacketsToSendPerClient = new System.Windows.Forms.TextBox();
            this.lblPacketsToSendPerClient = new System.Windows.Forms.Label();
            this.lblPacketsSizeInBytes = new System.Windows.Forms.Label();
            this.txtbxPacketSizeInBytes = new System.Windows.Forms.TextBox();
            this.txtbxDelayBetweenSends = new System.Windows.Forms.TextBox();
            this.lblDelayBetweenSends = new System.Windows.Forms.Label();
            this.grpbxClientsSettings = new System.Windows.Forms.GroupBox();
            this.grpbxServerSettings = new System.Windows.Forms.GroupBox();
            this.lblAccepterLatency = new System.Windows.Forms.Label();
            this.txtbxAccepterLatency = new System.Windows.Forms.TextBox();
            this.chkbxTODO = new System.Windows.Forms.CheckedListBox();
            this.pbarTask = new System.Windows.Forms.ProgressBar();
            this.grpbxBenchmarkResults_clients = new System.Windows.Forms.GroupBox();
            this.lstbxBMResults_clients = new System.Windows.Forms.ListBox();
            this.grpbxBenchmarkResults_server = new System.Windows.Forms.GroupBox();
            this.lstbxBMResults_server = new System.Windows.Forms.ListBox();
            this.grpbxClientsSettings.SuspendLayout();
            this.grpbxServerSettings.SuspendLayout();
            this.grpbxBenchmarkResults_clients.SuspendLayout();
            this.grpbxBenchmarkResults_server.SuspendLayout();
            this.SuspendLayout();
            // 
            // btnStartTesting
            // 
            this.btnStartTesting.Location = new System.Drawing.Point(12, 198);
            this.btnStartTesting.Name = "btnStartTesting";
            this.btnStartTesting.Size = new System.Drawing.Size(75, 23);
            this.btnStartTesting.TabIndex = 0;
            this.btnStartTesting.Text = "Start Testing";
            this.btnStartTesting.UseVisualStyleBackColor = true;
            this.btnStartTesting.Click += new System.EventHandler(this.btnStartTesting_Click);
            // 
            // lblClientsAmount
            // 
            this.lblClientsAmount.AutoSize = true;
            this.lblClientsAmount.Location = new System.Drawing.Point(6, 22);
            this.lblClientsAmount.Name = "lblClientsAmount";
            this.lblClientsAmount.Size = new System.Drawing.Size(80, 13);
            this.lblClientsAmount.TabIndex = 1;
            this.lblClientsAmount.Text = "Clients Amount:";
            // 
            // txtbxClientsAmount
            // 
            this.txtbxClientsAmount.Location = new System.Drawing.Point(212, 19);
            this.txtbxClientsAmount.Name = "txtbxClientsAmount";
            this.txtbxClientsAmount.Size = new System.Drawing.Size(59, 20);
            this.txtbxClientsAmount.TabIndex = 2;
            this.txtbxClientsAmount.Text = "100";
            // 
            // txtbxPacketsToSendPerClient
            // 
            this.txtbxPacketsToSendPerClient.Location = new System.Drawing.Point(212, 45);
            this.txtbxPacketsToSendPerClient.Name = "txtbxPacketsToSendPerClient";
            this.txtbxPacketsToSendPerClient.Size = new System.Drawing.Size(59, 20);
            this.txtbxPacketsToSendPerClient.TabIndex = 4;
            this.txtbxPacketsToSendPerClient.Text = "10";
            // 
            // lblPacketsToSendPerClient
            // 
            this.lblPacketsToSendPerClient.AutoSize = true;
            this.lblPacketsToSendPerClient.Location = new System.Drawing.Point(6, 48);
            this.lblPacketsToSendPerClient.Name = "lblPacketsToSendPerClient";
            this.lblPacketsToSendPerClient.Size = new System.Drawing.Size(133, 13);
            this.lblPacketsToSendPerClient.TabIndex = 3;
            this.lblPacketsToSendPerClient.Text = "Packets to send per client:";
            // 
            // lblPacketsSizeInBytes
            // 
            this.lblPacketsSizeInBytes.AutoSize = true;
            this.lblPacketsSizeInBytes.Location = new System.Drawing.Point(6, 74);
            this.lblPacketsSizeInBytes.Name = "lblPacketsSizeInBytes";
            this.lblPacketsSizeInBytes.Size = new System.Drawing.Size(105, 13);
            this.lblPacketsSizeInBytes.TabIndex = 5;
            this.lblPacketsSizeInBytes.Text = "Packet size in Bytes:";
            // 
            // txtbxPacketSizeInBytes
            // 
            this.txtbxPacketSizeInBytes.Location = new System.Drawing.Point(212, 71);
            this.txtbxPacketSizeInBytes.Name = "txtbxPacketSizeInBytes";
            this.txtbxPacketSizeInBytes.Size = new System.Drawing.Size(59, 20);
            this.txtbxPacketSizeInBytes.TabIndex = 6;
            this.txtbxPacketSizeInBytes.Text = "50";
            // 
            // txtbxDelayBetweenSends
            // 
            this.txtbxDelayBetweenSends.Location = new System.Drawing.Point(212, 97);
            this.txtbxDelayBetweenSends.Name = "txtbxDelayBetweenSends";
            this.txtbxDelayBetweenSends.Size = new System.Drawing.Size(59, 20);
            this.txtbxDelayBetweenSends.TabIndex = 8;
            this.txtbxDelayBetweenSends.Text = "0";
            // 
            // lblDelayBetweenSends
            // 
            this.lblDelayBetweenSends.AutoSize = true;
            this.lblDelayBetweenSends.Location = new System.Drawing.Point(6, 100);
            this.lblDelayBetweenSends.Name = "lblDelayBetweenSends";
            this.lblDelayBetweenSends.Size = new System.Drawing.Size(196, 13);
            this.lblDelayBetweenSends.TabIndex = 7;
            this.lblDelayBetweenSends.Text = "Delay Between sends (1000=1 second):";
            // 
            // grpbxClientsSettings
            // 
            this.grpbxClientsSettings.Controls.Add(this.lblClientsAmount);
            this.grpbxClientsSettings.Controls.Add(this.txtbxDelayBetweenSends);
            this.grpbxClientsSettings.Controls.Add(this.txtbxClientsAmount);
            this.grpbxClientsSettings.Controls.Add(this.lblDelayBetweenSends);
            this.grpbxClientsSettings.Controls.Add(this.lblPacketsToSendPerClient);
            this.grpbxClientsSettings.Controls.Add(this.txtbxPacketSizeInBytes);
            this.grpbxClientsSettings.Controls.Add(this.txtbxPacketsToSendPerClient);
            this.grpbxClientsSettings.Controls.Add(this.lblPacketsSizeInBytes);
            this.grpbxClientsSettings.Location = new System.Drawing.Point(12, 12);
            this.grpbxClientsSettings.Name = "grpbxClientsSettings";
            this.grpbxClientsSettings.Size = new System.Drawing.Size(277, 126);
            this.grpbxClientsSettings.TabIndex = 9;
            this.grpbxClientsSettings.TabStop = false;
            this.grpbxClientsSettings.Text = "Clients Settings";
            // 
            // grpbxServerSettings
            // 
            this.grpbxServerSettings.Controls.Add(this.lblAccepterLatency);
            this.grpbxServerSettings.Controls.Add(this.txtbxAccepterLatency);
            this.grpbxServerSettings.Location = new System.Drawing.Point(12, 144);
            this.grpbxServerSettings.Name = "grpbxServerSettings";
            this.grpbxServerSettings.Size = new System.Drawing.Size(277, 48);
            this.grpbxServerSettings.TabIndex = 10;
            this.grpbxServerSettings.TabStop = false;
            this.grpbxServerSettings.Text = "Server Settings";
            // 
            // lblAccepterLatency
            // 
            this.lblAccepterLatency.AutoSize = true;
            this.lblAccepterLatency.Location = new System.Drawing.Point(6, 22);
            this.lblAccepterLatency.Name = "lblAccepterLatency";
            this.lblAccepterLatency.Size = new System.Drawing.Size(177, 13);
            this.lblAccepterLatency.TabIndex = 3;
            this.lblAccepterLatency.Text = "Accepter Latency (1000=1 second):";
            // 
            // txtbxAccepterLatency
            // 
            this.txtbxAccepterLatency.Location = new System.Drawing.Point(212, 19);
            this.txtbxAccepterLatency.Name = "txtbxAccepterLatency";
            this.txtbxAccepterLatency.Size = new System.Drawing.Size(59, 20);
            this.txtbxAccepterLatency.TabIndex = 4;
            this.txtbxAccepterLatency.Text = "1000";
            // 
            // chkbxTODO
            // 
            this.chkbxTODO.Enabled = false;
            this.chkbxTODO.FormattingEnabled = true;
            this.chkbxTODO.Items.AddRange(new object[] {
            "Initializing Server & Clients",
            "Connecting Clients",
            "Sending Packets",
            "Recieving Pakces",
            "Disconnecting Clients",
            "Finishing & Calculating Results"});
            this.chkbxTODO.Location = new System.Drawing.Point(12, 228);
            this.chkbxTODO.Name = "chkbxTODO";
            this.chkbxTODO.Size = new System.Drawing.Size(277, 154);
            this.chkbxTODO.TabIndex = 11;
            // 
            // pbarTask
            // 
            this.pbarTask.Location = new System.Drawing.Point(94, 199);
            this.pbarTask.Name = "pbarTask";
            this.pbarTask.Size = new System.Drawing.Size(195, 23);
            this.pbarTask.TabIndex = 12;
            // 
            // grpbxBenchmarkResults_clients
            // 
            this.grpbxBenchmarkResults_clients.Controls.Add(this.lstbxBMResults_clients);
            this.grpbxBenchmarkResults_clients.Location = new System.Drawing.Point(295, 12);
            this.grpbxBenchmarkResults_clients.Name = "grpbxBenchmarkResults_clients";
            this.grpbxBenchmarkResults_clients.Size = new System.Drawing.Size(277, 180);
            this.grpbxBenchmarkResults_clients.TabIndex = 13;
            this.grpbxBenchmarkResults_clients.TabStop = false;
            this.grpbxBenchmarkResults_clients.Text = "Benchmark Results (Clients)";
            // 
            // lstbxBMResults_clients
            // 
            this.lstbxBMResults_clients.FormattingEnabled = true;
            this.lstbxBMResults_clients.Location = new System.Drawing.Point(6, 27);
            this.lstbxBMResults_clients.Name = "lstbxBMResults_clients";
            this.lstbxBMResults_clients.Size = new System.Drawing.Size(265, 147);
            this.lstbxBMResults_clients.TabIndex = 2;
            // 
            // grpbxBenchmarkResults_server
            // 
            this.grpbxBenchmarkResults_server.Controls.Add(this.lstbxBMResults_server);
            this.grpbxBenchmarkResults_server.Location = new System.Drawing.Point(295, 202);
            this.grpbxBenchmarkResults_server.Name = "grpbxBenchmarkResults_server";
            this.grpbxBenchmarkResults_server.Size = new System.Drawing.Size(277, 180);
            this.grpbxBenchmarkResults_server.TabIndex = 14;
            this.grpbxBenchmarkResults_server.TabStop = false;
            this.grpbxBenchmarkResults_server.Text = "Benchmark Results (Server)";
            // 
            // lstbxBMResults_server
            // 
            this.lstbxBMResults_server.FormattingEnabled = true;
            this.lstbxBMResults_server.Location = new System.Drawing.Point(6, 27);
            this.lstbxBMResults_server.Name = "lstbxBMResults_server";
            this.lstbxBMResults_server.Size = new System.Drawing.Size(265, 147);
            this.lstbxBMResults_server.TabIndex = 3;
            // 
            // frmMain
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(581, 391);
            this.Controls.Add(this.grpbxBenchmarkResults_server);
            this.Controls.Add(this.grpbxBenchmarkResults_clients);
            this.Controls.Add(this.pbarTask);
            this.Controls.Add(this.chkbxTODO);
            this.Controls.Add(this.grpbxServerSettings);
            this.Controls.Add(this.grpbxClientsSettings);
            this.Controls.Add(this.btnStartTesting);
            this.Name = "frmMain";
            this.Text = "Benchmark";
            this.grpbxClientsSettings.ResumeLayout(false);
            this.grpbxClientsSettings.PerformLayout();
            this.grpbxServerSettings.ResumeLayout(false);
            this.grpbxServerSettings.PerformLayout();
            this.grpbxBenchmarkResults_clients.ResumeLayout(false);
            this.grpbxBenchmarkResults_server.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Button btnStartTesting;
        private System.Windows.Forms.Label lblClientsAmount;
        private System.Windows.Forms.TextBox txtbxClientsAmount;
        private System.Windows.Forms.TextBox txtbxPacketsToSendPerClient;
        private System.Windows.Forms.Label lblPacketsToSendPerClient;
        private System.Windows.Forms.Label lblPacketsSizeInBytes;
        private System.Windows.Forms.TextBox txtbxPacketSizeInBytes;
        private System.Windows.Forms.TextBox txtbxDelayBetweenSends;
        private System.Windows.Forms.Label lblDelayBetweenSends;
        private System.Windows.Forms.GroupBox grpbxClientsSettings;
        private System.Windows.Forms.GroupBox grpbxServerSettings;
        private System.Windows.Forms.Label lblAccepterLatency;
        private System.Windows.Forms.TextBox txtbxAccepterLatency;
        private System.Windows.Forms.CheckedListBox chkbxTODO;
        private System.Windows.Forms.ProgressBar pbarTask;
        private System.Windows.Forms.GroupBox grpbxBenchmarkResults_clients;
        private System.Windows.Forms.GroupBox grpbxBenchmarkResults_server;
        private System.Windows.Forms.ListBox lstbxBMResults_clients;
        private System.Windows.Forms.ListBox lstbxBMResults_server;
    }
}

