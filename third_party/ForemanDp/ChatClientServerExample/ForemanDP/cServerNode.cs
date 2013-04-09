using System;
using System.Collections.Generic;
using System.Text;
using System.Threading;

namespace ForemanDP
{
    internal class ServerNode
    {
        private Guid nodeId = Guid.NewGuid();

        private List<BasicWorker> workers = new List<BasicWorker>();

        public delegate void DelegateServerNodeNotification(BasicWorker worker, object data);
        public event DelegateServerNodeNotification OnServerNodeNotifyEvent;

        public delegate void DelegateServerNodeWorkerLatencyChanged(BasicWorker worker, TimeSpan oldLatency);
        public event DelegateServerNodeWorkerLatencyChanged OnServerNodeWorkerLatencyChanged;

        private TimeSpan latency = TimeSpan.MinValue;
        private Thread threadNode = null;
        private bool bExitThreadNode = false;

        private bool bEliminationRequestSent = false;

        public Guid NodeId
        {
            get { return nodeId; }
        }

        /// <summary>
        /// Get latency information - the amount of time required to signal all workers to function.
        /// </summary>
        public TimeSpan Latency
        {
            get { return latency; }
        }

        public Thread ThreadNode
        {
            get { return threadNode; }
        }

        public bool Idle
        {
            get { return workers.Count == 0; }
        }

        public ServerNode()
        {
            threadNode = new Thread(new ThreadStart(NodeFunction));
            threadNode.IsBackground = true;
        }

        public void Start()
        {
            if (!threadNode.IsAlive)
            {
                bExitThreadNode = false;
                threadNode.Start();
            }
        }

        public void Stop()
        {
            bExitThreadNode = true;
        }


        private enum NodeOperationEnum
        {
            None=0,
            AddWorker=1,
            RemoveWorker=2,
            RemoveAll=3
        }

        private class NodeOperation
        {
            private NodeOperationEnum nodeOperationEnum;
            private object relatedObject;

            public NodeOperationEnum NodeOperationEnum
            {
                get { return nodeOperationEnum; }
                set { nodeOperationEnum = value; }
            }

            public object RelatedObject
            {
                get { return relatedObject; }
                set { relatedObject = value; }
            }

            public NodeOperation(NodeOperationEnum nodeOperationEnum, object relatedObject)
            {
                this.nodeOperationEnum = nodeOperationEnum;
                this.relatedObject = relatedObject;
            }
        }

        private Queue<NodeOperation> qNodeOpeations = new Queue<NodeOperation>();

        /// <summary>
        /// Adds a worker to the Node, the ServerNode will add this worker to the scan.
        /// </summary>
        /// <param name="worker">Any type derived from BasicWorker.</param>
        public void AddWorker(BasicWorker worker)
        {
            qNodeOpeations.Enqueue(new NodeOperation(NodeOperationEnum.AddWorker, worker));
        }

        public void RemoveWorker(BasicWorker worker)
        {
            qNodeOpeations.Enqueue(new NodeOperation(NodeOperationEnum.RemoveWorker, worker));
        }

        public void RemoveAllWorkers()
        {
            qNodeOpeations.Enqueue(new NodeOperation(NodeOperationEnum.RemoveAll, null));
        }

        void worker_OnWorkerNotifyEvent(BasicWorker worker, object data)
        {           
            if (OnServerNodeNotifyEvent != null) OnServerNodeNotifyEvent(worker, data);
        }

        void ServerNode_OnWorkerLatencyChanged(BasicWorker worker, TimeSpan oldLatency)
        {
            //if the latnecy of the worker have changed send the worker to node reassignment.
            qNodeOpeations.Enqueue(new NodeOperation(NodeOperationEnum.RemoveWorker, worker));
            if (OnServerNodeWorkerLatencyChanged != null) OnServerNodeWorkerLatencyChanged(worker, oldLatency);
        }

        public void NodeFunction()
        {
            while (!bExitThreadNode)
            {
                //check for euque elements
                if (qNodeOpeations.Count > 0)
                {
                    NodeOperation operation = qNodeOpeations.Dequeue();
                    switch (operation.NodeOperationEnum)
                    {
                        case NodeOperationEnum.None:
                            break;
                        case NodeOperationEnum.AddWorker:
                            ((BasicWorker)operation.RelatedObject).OnWorkerNotifyEvent +=
                                new BasicWorker.DelegateWorkerNotification(worker_OnWorkerNotifyEvent);
                            ((BasicWorker)operation.RelatedObject).OnWorkerLatencyChanged +=
                                new BasicWorker.DelegateWorkerLatencyChanged(ServerNode_OnWorkerLatencyChanged);
                            workers.Add((BasicWorker)operation.RelatedObject);
                            break;
                        case NodeOperationEnum.RemoveWorker:
                            ((BasicWorker)operation.RelatedObject).OnWorkerNotifyEvent -=
                                new BasicWorker.DelegateWorkerNotification(worker_OnWorkerNotifyEvent);
                            ((BasicWorker)operation.RelatedObject).OnWorkerLatencyChanged -=
                                new BasicWorker.DelegateWorkerLatencyChanged(ServerNode_OnWorkerLatencyChanged);
                            workers.Remove((BasicWorker)operation.RelatedObject);
                            bEliminationRequestSent = false;
                            break;
                        case NodeOperationEnum.RemoveAll:
                            workers.Clear();
                            bEliminationRequestSent = false;
                            break;
                    }
                }
                DateTime oldTime = DateTime.Now;
                if (workers.Count == 0)
                {
                    //notify BasicServer to eliminate this node since it's idle.
                    if (OnServerNodeNotifyEvent != null && !bEliminationRequestSent)
                    {
                        bEliminationRequestSent = true;
                        OnServerNodeNotifyEvent(null, nodeId);
                    }
                }
                else foreach (BasicWorker worker in workers) worker.work();    
                latency = DateTime.Now - oldTime; //update the latency information.
            }
        }
    }
}
