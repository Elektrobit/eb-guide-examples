////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Elektrobit Automotive GmbH
// Alle Rechte vorbehalten. All Rights Reserved.
// Information contained herein is subject to change without notice.
// Elektrobit retains ownership and all other rights in the software and each
// component thereof.
// Any reproduction of the software or components thereof without the prior
// written permission of Elektrobit is prohibited.
////////////////////////////////////////////////////////////////////////////////

namespace MonitorRemoteViewPluginTest.Tests.RemoteViewViewModelTests
{
    using Elektrobit.Guide.Monitor.Service;
    using Elektrobit.Guide.Monitor.Service.ScreenshotService;
    using Elektrobit.Guide.Monitor.Ui.Dialog;
    using Elektrobit.Guide.Monitor.Utilities;
    using Elektrobit.Guide.Scheduler;

    using FakeItEasy;
    using MonitorRemoteViewPlugin.Utilities.ContinuousScreenshot;
    using MonitorRemoteViewPlugin.Utilities.FrameDelayConverter;
    using MonitorRemoteViewPlugin.ViewModels;
    using NUnit.Framework;

    public class RemoteViewViewModelTestBase
    {
        public IScreenshotService ScreenshotService;
        public IScreenshotSavingUtility ScreenshotSavingUtility;
        public ILoggerService LoggerService;
        public IConnectionService ConnectionService;
        public ITaskSchedulerProvider SchedulerProvider;
        public IEventService EventService;
        public IContinuousScreenshotController ContinuousScreenshotController;
        public IFrameDelayConverter FrameDelayConverter;
        public IContinuousScreenshotRunner ContinuousScreenshotRunner;
        public IMonitorDialog MonitorDialog;

        public RemoteViewViewModel ViewModel;

        [SetUp]
        public void SetUp()
        {
            ScreenshotService = A.Fake<IScreenshotService>();
            ScreenshotSavingUtility = A.Fake<IScreenshotSavingUtility>();
            LoggerService = A.Fake<ILoggerService>();
            ConnectionService = A.Fake<IConnectionService>();
            SchedulerProvider = A.Fake<ITaskSchedulerProvider>();
            EventService = A.Fake<IEventService>();
            ContinuousScreenshotController = A.Fake<IContinuousScreenshotController>();
            FrameDelayConverter = A.Fake<IFrameDelayConverter>();
            ContinuousScreenshotRunner = A.Fake<IContinuousScreenshotRunner>();
            MonitorDialog = A.Fake<IMonitorDialog>();

            ViewModel = new RemoteViewViewModel(ScreenshotService, ScreenshotSavingUtility, LoggerService, ConnectionService, SchedulerProvider, EventService, ContinuousScreenshotController, FrameDelayConverter, MonitorDialog);
        }

    }
}