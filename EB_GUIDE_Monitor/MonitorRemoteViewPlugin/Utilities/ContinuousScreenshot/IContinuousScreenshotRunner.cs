////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Elektrobit Automotive GmbH
// Alle Rechte vorbehalten. All Rights Reserved.
// Information contained herein is subject to change without notice.
// Elektrobit retains ownership and all other rights in the software and each
// component thereof.
// Any reproduction of the software or components thereof without the prior
// written permission of Elektrobit is prohibited.
////////////////////////////////////////////////////////////////////////////////

namespace MonitorRemoteViewPlugin.Utilities.ContinuousScreenshot
{
    using System.ComponentModel;
    using System.Threading.Tasks;
    using Elektrobit.Guide.Monitor.Service.ScreenshotService;
    using MonitorRemoteViewPlugin.Utilities.Cancellation;

    public interface IContinuousScreenshotRunner : INotifyPropertyChanged
    {
        uint Delay { get; set; }

        Task Run(uint sceneId);

        Task Stop();

        bool IsRunning { get; }

        IScreenshot Screenshot { get;}

        ICancellation Cancellation { get; }
    }
}