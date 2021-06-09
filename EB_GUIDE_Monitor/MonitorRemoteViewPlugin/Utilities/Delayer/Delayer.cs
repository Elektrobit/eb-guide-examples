////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Elektrobit Automotive GmbH
// Alle Rechte vorbehalten. All Rights Reserved.
// Information contained herein is subject to change without notice.
// Elektrobit retains ownership and all other rights in the software and each
// component thereof.
// Any reproduction of the software or components thereof without the prior
// written permission of Elektrobit is prohibited.
////////////////////////////////////////////////////////////////////////////////

namespace MonitorRemoteViewPlugin.Utilities.Delayer
{
    using System;
    using System.ComponentModel.Composition;
    using System.Threading.Tasks;
    using MonitorRemoteViewPlugin.Utilities.Cancellation;

    [Export(typeof(IDelayer))]
    public class Delayer : IDelayer
    {
        public Task FrameDelay(uint milliSeconds, ICancellation cancellation)
        {
            return Task.Delay(TimeSpan.FromMilliseconds(milliSeconds), cancellation.Source.Token);
        }
    }
}