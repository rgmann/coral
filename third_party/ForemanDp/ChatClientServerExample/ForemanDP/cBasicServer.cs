using System;
using System.Collections.Generic;
using System.Text;
using System.Threading;
using System.Collections.ObjectModel;

namespace ForemanDP
{
    /// <summary>
    /// This class is targeted to orginize workers by latency,
    /// It will open threads dynamiclly to meet the latnecy requirment per worker or
    /// will add a worker to a current thread if will meet the worker latency requirement.
    /// </summary>
    public class BasicServer
    {
        private bool bServerStarted = false;
        private bool bServerPaused = false;
        private object oLocker = new object(); //Used to sync calls to public methods from the outside.

        //This list type can send events when somthing is changed in it.
        private ObservableCollection<BasicWorker> workers = new ObservableCollection<BasicWorker>();
        private List<ServerNode> serverNodes = new List<ServerNode>();
        private object oServerNodesLocker = new object();

        public delegate void DelegateServerNotification(BasicWorker worker, object data);
        public event DelegateServerNotification OnServerNotifyEvent;

        public ObservableCollection<BasicWorker> Workers
        {
            get { return workers; }
            set { workers = value; }
        }

        public BasicServer() 
        {
            //Hook to the list observer notification events.
            workers.CollectionChanged += 
                new System.Collections.Specialized.NotifyCollectionChangedEventHandler(
                workers_CollectionChanged);
        }

        public void Start()
        {
            lock (oLocker)
            {
                if (bServerStarted) throw new Exception("Error: Start invoked when server already started.");
                bServerStarted = true;
                foreach (BasicWorker worker in workers) AddWorker(worker);
            }
        }

        public void Stop()
        {
            lock (oLocker)
            {
                if (!bServerStarted) throw new Exception("Error: Stop invoked when server already stopped.");
                bServerStarted = false;
                bServerPaused = false;
                foreach (ServerNode node in serverNodes)
                {
                    node.Stop();
                    node.RemoveAllWorkers();
                }
            }
        }

        public void Pause()
        {
            lock (oLocker)
            {
                if (!bServerStarted) throw new Exception("Error: Pause invoked when server not started.");
                if (bServerPaused) throw new Exception("Error: Pause invoked when server already paused.");
                bServerPaused = true;
                foreach (ServerNode node in serverNodes) node.Stop();
            }
        }

        public void Continue()
        {
            lock (oLocker)
            {
                if (!bServerStarted) throw new Exception("Error: Conntinue invoked when server not started.");
                if (!bServerPaused) throw new Exception("Error: Continue invoked when server not paused.");
                bServerPaused = false;
                foreach (ServerNode node in serverNodes) node.Start();
            }
        }

        /// <summary>
        /// Adds a worker to a node from the nodeslist or open a new node for the worker.
        /// The action taken depends on the required latnecy of the worker and the current nodes latencies.
        /// </summary>
        /// <param name="worker">The worker to add.</param>
        private void AddWorker(BasicWorker worker)
        {
            int iBestNodeIndex = 0;
            lock (oServerNodesLocker)
            {
                TimeSpan lowestNodeLatency = TimeSpan.MaxValue;
                for (int iScan = 0; iScan < serverNodes.Count; iScan++)
                    if (serverNodes[iScan].Latency < lowestNodeLatency)
                    {
                        iBestNodeIndex = iScan;
                        lowestNodeLatency = serverNodes[iBestNodeIndex].Latency;
                    }
                if (lowestNodeLatency > worker.RequiredLatency)
                {   //Create a new node.
                    ServerNode NewNode = new ServerNode();
                    NewNode.OnServerNodeNotifyEvent += //Hook to the node events.
                        new ServerNode.DelegateServerNodeNotification(
                        NewNode_OnServerNodeNotifyEvent);
                    NewNode.OnServerNodeWorkerLatencyChanged += 
                        new ServerNode.DelegateServerNodeWorkerLatencyChanged(
                        NewNode_OnServerNodeWorkerLatencyChanged);
                    serverNodes.Add(NewNode); //Add the new node to the list.
                    iBestNodeIndex = serverNodes.Count - 1;
                    if (bServerStarted && !bServerPaused) NewNode.Start();
                }
                serverNodes[iBestNodeIndex].AddWorker(worker);
            }
        }

        void NewNode_OnServerNodeNotifyEvent(BasicWorker worker, object data)
        {
            if (worker == null) //this means the node is reporting, node id is in the data object.
            {
                lock (oServerNodesLocker)
                {
                    for(int iScan=0;iScan<serverNodes.Count;iScan++)
                    {
                        if(((Guid)data).CompareTo(serverNodes[iScan].NodeId)==0)
                        {
                            if (serverNodes[iScan].Idle)
                            {
                                serverNodes[iScan].Stop(); 
                                serverNodes[iScan].OnServerNodeNotifyEvent -=
                                    new ServerNode.DelegateServerNodeNotification(
                                    NewNode_OnServerNodeNotifyEvent);
                                serverNodes[iScan].OnServerNodeWorkerLatencyChanged -=
                                    new ServerNode.DelegateServerNodeWorkerLatencyChanged(
                                    NewNode_OnServerNodeWorkerLatencyChanged);
                                serverNodes.RemoveAt(iScan);
                            }
                            break;
                        }
                    }
                }
            }
            else if (OnServerNotifyEvent != null) OnServerNotifyEvent.BeginInvoke(worker, data, null, null);
        }

        //Warp an internal event.
        public struct WorkerLatencyInformationChanged
        {
            private TimeSpan oldLatency;

            public TimeSpan OldLatency
            {
                get { return oldLatency; }
                set { oldLatency = value; }
            }

            public WorkerLatencyInformationChanged(TimeSpan workerOldLatency)
            {
                oldLatency = workerOldLatency;
            }
        }

        void NewNode_OnServerNodeWorkerLatencyChanged(BasicWorker worker, TimeSpan oldLatency)
        {
            AddWorker(worker);
            if (OnServerNotifyEvent != null) OnServerNotifyEvent.BeginInvoke(worker,
                new WorkerLatencyInformationChanged(oldLatency), null, null);
        }

        //When the user of this class add or change somthing in the colletion an event will call this method.
        void workers_CollectionChanged(object sender, System.Collections.Specialized.NotifyCollectionChangedEventArgs e)
        {
            switch (e.Action)
            {
                case System.Collections.Specialized.NotifyCollectionChangedAction.Add:
                    if (bServerStarted) foreach (BasicWorker worker in e.NewItems) AddWorker(worker);
                    break;
                case System.Collections.Specialized.NotifyCollectionChangedAction.Remove:
                    if (bServerStarted)
                        foreach (BasicWorker worker in e.OldItems)
                            foreach (ServerNode serverNode in serverNodes) serverNode.RemoveWorker(worker);
                    break;
                case System.Collections.Specialized.NotifyCollectionChangedAction.Move:
                case System.Collections.Specialized.NotifyCollectionChangedAction.Replace:
                case System.Collections.Specialized.NotifyCollectionChangedAction.Reset:
                    throw new Exception("The Workers property is restricted to adding or removing workers only.");
            }
        }
    }
}
