using System;
using System.Collections.Generic;
using System.Text;

namespace ForemanDP
{
    public abstract class BasicWorker
    {
        private string key = null;
        private TimeSpan requiredLatency;

        protected bool alreadyWorking = false;
        protected bool enable = false;

        public delegate void DelegateWorkerNotification(BasicWorker worker, object data);
        public event DelegateWorkerNotification OnWorkerNotifyEvent;

        public delegate void DelegateWorkerLatencyChanged(BasicWorker worker, TimeSpan oldLatency);
        public event DelegateWorkerLatencyChanged OnWorkerLatencyChanged;

        public BasicWorker(string key, TimeSpan requiredLatency)
        {
            this.key = key;
            this.requiredLatency = requiredLatency;
        }

        /// <summary>
        /// Property to identify the worker.
        /// </summary>
        public string Key
        {
            get { return key; }
            set { key = value; }
        }

        public TimeSpan RequiredLatency
        {
            get { return requiredLatency; }
            set 
            {
                TimeSpan oldLatency = requiredLatency;
                requiredLatency = value;
                RaiseWorkerLatencyChanged(this, oldLatency);
            }
        }

        public bool Enable
        {
            get { return enable; }
            set { enable = value; }
        }

        public void work()
        {
            if (enable)
            {
                if (!alreadyWorking)
                {
                    AtWorkStart();
                    alreadyWorking = true;
                }
                else AtWorkRoutine();
            }
            else if (alreadyWorking)
            {
                AtWorkFinish();
                alreadyWorking = false;
            }
        }

        public abstract void AtWorkStart();
        public abstract void AtWorkRoutine();
        public abstract void AtWorkFinish();

        protected void RaiseWorkerNotification(BasicWorker worker, object data)
        {
            if (OnWorkerNotifyEvent != null) OnWorkerNotifyEvent(worker, data);
        }

        protected void RaiseWorkerLatencyChanged(BasicWorker worker, TimeSpan oldLatency)
        {
            if (OnWorkerLatencyChanged != null) OnWorkerLatencyChanged(worker, oldLatency);
        }
    }
}
