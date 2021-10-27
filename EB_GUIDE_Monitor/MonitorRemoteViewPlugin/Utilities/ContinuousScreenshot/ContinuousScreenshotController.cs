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
    using System.ComponentModel;
    using System.ComponentModel.Composition;
    using System.Threading;
    using System.Threading.Tasks;
    using Elektrobit.Guide.Monitor.Service.ScreenshotService;
    using Elektrobit.Guide.Utilities;
    using Prism.Mvvm;

    [Export(typeof(IContinuousScreenshotController))]
    internal class ContinuousScreenshotController : BindableBase, IContinuousScreenshotController, IDisposable
    {
        private readonly IContinuousScreenshotRunner _continuousScreenshotRunner;
        private IScreenshot _screenshot;
        private uint _sceneId;
        public uint Delay { get => _continuousScreenshotRunner.Delay; set => _continuousScreenshotRunner.Delay = value; }
        public bool IsRunning => _continuousScreenshotRunner.IsRunning;

        public IScreenshot Screenshot
        {
            get => _screenshot;
            private set => SetProperty(ref _screenshot, value);
        }

        [ImportingConstructor]
        public ContinuousScreenshotController(IContinuousScreenshotRunner continuousScreenshotRunner)
        {
            Precondition.IsNotNull(continuousScreenshotRunner, nameof(continuousScreenshotRunner));

            _continuousScreenshotRunner = continuousScreenshotRunner;

            Register();
        }

        public async Task ToggleAsync(uint sceneId)
        {
            if (_continuousScreenshotRunner.IsRunning)
            {
                await StopAsync(sceneId);
            }
            else
            {
                await StartAsync(sceneId);
            }
        }

        public async Task StartAsync(uint sceneId, CancellationTokenSource tokenSource = null)
        {
            if (_continuousScreenshotRunner.IsRunning)
            {
                throw new AlreadyRunningException();
            }

            _sceneId = sceneId;
            await _continuousScreenshotRunner.Run(sceneId);
        }

        public async Task StopAsync(uint sceneId)
        {
            if (_sceneId != sceneId)
            {
                throw new SceneIdMismatchException();
            }

            await _continuousScreenshotRunner.Stop();
        }

        private void Register()
        {
            _continuousScreenshotRunner.PropertyChanged += ContinuousScreenshotChanged;
        }

        private void Unregister()
        {
            _continuousScreenshotRunner.PropertyChanged -= ContinuousScreenshotChanged;
        }

        private void ContinuousScreenshotChanged(object sender, PropertyChangedEventArgs e)
        {
            if (sender != _continuousScreenshotRunner)
            {
                return;
            }

            if (e.PropertyName != "Screenshot")
            {
                return;
            }

            Screenshot = _continuousScreenshotRunner.Screenshot;
        }

        public void Dispose()
        {
            Unregister();
        }
    }
}