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
    using System.Threading;
    using System.Threading.Tasks;

    public interface ICancellation
    {
        void Reset();

        bool IsRunning { get; }

        CancellationTokenSource Source { get; }

        Task Cancel();
    }
}
