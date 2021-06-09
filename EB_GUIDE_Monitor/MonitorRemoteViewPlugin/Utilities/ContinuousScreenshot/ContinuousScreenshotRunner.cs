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
    using System;
    using System.Threading.Tasks;
    using Elektrobit.Guide.Monitor.Service.ScreenshotService;
    using Elektrobit.Guide.Utilities;
    using MonitorRemoteViewPlugin.Utilities.Cancellation;
    using MonitorRemoteViewPlugin.Utilities.Delayer;
    using Prism.Mvvm;

    public class ContinuousScreenshotRunner : BindableBase, IContinuousScreenshotRunner
    {
        private readonly IScreenshotService _screenshotService;
        private readonly IDelayer _delayer;
        private IScreenshot _screenshot;
        public ICancellation Cancellation { get; private set; }
        public uint Delay { get; set; }
        public bool IsRunning => Cancellation.IsRunning;

        public IScreenshot Screenshot
        {
            get => _screenshot;
            private set => SetProperty(ref _screenshot, value);
        }

        public ContinuousScreenshotRunner(
            ICancellation cancellation,
            IScreenshotService screenshotService,
            IDelayer delayer)
        {
            Precondition.IsNotNull(screenshotService, nameof(screenshotService));
            Precondition.IsNotNull(delayer, nameof(delayer));
            Precondition.IsNotNull(cancellation, nameof(cancellation));

            Cancellation = cancellation;
            _screenshotService = screenshotService;
            _delayer = delayer;
        }

        public async Task Run(uint sceneId)
        {
            Cancellation.Reset();
            while (IsRunning)
            {
                Screenshot = await _screenshotService.GetScreenshotAsync(sceneId);
                try
                {
                    await _delayer.FrameDelay(Delay, Cancellation);
                }
                catch (TaskCanceledException)
                {
                    // ignore task canceled exception
                }
            }
        }

        public async Task Stop()
        {
            await Cancellation.Cancel();
        }
    }
}