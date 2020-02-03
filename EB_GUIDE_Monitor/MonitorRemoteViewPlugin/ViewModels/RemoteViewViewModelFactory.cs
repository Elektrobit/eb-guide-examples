////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Elektrobit Automotive GmbH
// Alle Rechte vorbehalten. All Rights Reserved.
// Information contained herein is subject to change without notice.
// Elektrobit retains ownership and all other rights in the software and each
// component thereof.
// Any reproduction of the software or components thereof without the prior
// written permission of Elektrobit is prohibited.
////////////////////////////////////////////////////////////////////////////////


namespace MonitorRemoteViewPlugin.ViewModels
{
    using System.ComponentModel.Composition;
    using Elektrobit.Guide.Monitor.Service;
    using Elektrobit.Guide.Monitor.Service.ScreenshotService;
    using Elektrobit.Guide.Monitor.Ui.Dialog;
    using Elektrobit.Guide.Monitor.Ui.Docking;
    using Elektrobit.Guide.Monitor.Utilities;
    using Elektrobit.Guide.Scheduler;

    using MonitorRemoteViewPlugin.Utilities.ContinuousScreenshot;
    using MonitorRemoteViewPlugin.Utilities.FrameDelayConverter;

    [Export(typeof(IDockableComponentFactory))]
    public class RemoteViewViewModelFactory : IDockableComponentFactory
    {
        private readonly IScreenshotService _screenshotService;
        private readonly IScreenshotSavingUtility _screenshotSavingUtility;
        private readonly ILoggerService _loggerService;
        private readonly IConnectionService _connectionService;
        private readonly ITaskSchedulerProvider _schedulerProvider;
        private readonly IEventService _eventService;
        private readonly IFrameDelayConverter _frameDelayConverter;
        private readonly IMonitorDialog _monitorDialog;
        private readonly IContinuousScreenshotControllerFactory _screenshotControllerFactory;

        public string ContentIdBase { get; }
        public string Title { get; }

        [ImportingConstructor]
        public RemoteViewViewModelFactory(
            IScreenshotService screenshotService,
            IScreenshotSavingUtility screenshotSavingUtility,
            ILoggerService loggerService,
            IConnectionService connectionService,
            ITaskSchedulerProvider schedulerProvider,
            IEventService eventService,
            IFrameDelayConverter frameDelayConverter,
            IMonitorDialog monitorDialog,
            IContinuousScreenshotControllerFactory screenshotControllerFactory)
        {
            Title = "Model Viewer";
            ContentIdBase = "monitor_remote_view_extension_id";
            _screenshotService = screenshotService;
            _screenshotSavingUtility = screenshotSavingUtility;
            _loggerService = loggerService;
            _connectionService = connectionService;
            _schedulerProvider = schedulerProvider;
            _eventService = eventService;
            _frameDelayConverter = frameDelayConverter;
            _monitorDialog = monitorDialog;
            _screenshotControllerFactory = screenshotControllerFactory;
        }

        public IDockableComponentViewModel CreateComponent()
        {
            return new RemoteViewViewModel(_screenshotService, _screenshotSavingUtility, _loggerService, _connectionService, _schedulerProvider, _eventService, _screenshotControllerFactory.CreateController(), _frameDelayConverter, _monitorDialog);
        }
    }
}