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
    using System.Threading;
    using System.Threading.Tasks;
    using Elektrobit.Guide.Monitor.Service.ScreenshotService;

    public interface IContinuousScreenshotController : INotifyPropertyChanged
    {
        Task ToggleAsync(uint sceneId);

        IScreenshot Screenshot { get; }

        Task StartAsync(uint sceneId, CancellationTokenSource tokenSource = null);

        Task StopAsync(uint sceneId);

        uint Delay { get; set; }

        bool IsRunning { get; }
    }
}