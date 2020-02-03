////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Elektrobit Automotive GmbH
// Alle Rechte vorbehalten. All Rights Reserved.
// Information contained herein is subject to change without notice.
// Elektrobit retains ownership and all other rights in the software and each
// component thereof.
// Any reproduction of the software or components thereof without the prior
// written permission of Elektrobit is prohibited.
////////////////////////////////////////////////////////////////////////////////

namespace MonitorRemoteViewPlugin.Utilities.Cancellation
{
    using System;
    using System.Threading;
    using System.Threading.Tasks;

    public class Cancellation : ICancellation, IDisposable
    {
        public bool IsRunning => !Source?.IsCancellationRequested ?? false;

        public CancellationTokenSource Source { get; private set; }

        public void Reset()
        {
            if (IsRunning)
            {
                return;
            }

            Source = new CancellationTokenSource();
        }

        public Task Cancel()
        {
            Source.Cancel();
            return Task.CompletedTask;
        }

        public void Dispose()
        {
            Source?.Dispose();
        }
    }
}