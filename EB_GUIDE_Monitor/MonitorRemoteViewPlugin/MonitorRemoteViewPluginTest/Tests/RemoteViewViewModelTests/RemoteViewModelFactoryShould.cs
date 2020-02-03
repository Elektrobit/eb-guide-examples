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
    using System;
    using System.ComponentModel.Composition;
    using Elektrobit.Guide.Monitor.Service;
    using Elektrobit.Guide.Monitor.Service.ScreenshotService;
    using Elektrobit.Guide.Monitor.Ui.Dialog;
    using Elektrobit.Guide.Monitor.Ui.Docking;
    using Elektrobit.Guide.Monitor.Utilities;
    using Elektrobit.Guide.Scheduler;

    using FakeItEasy;
    using MonitorRemoteViewPlugin.Utilities.ContinuousScreenshot;
    using MonitorRemoteViewPlugin.Utilities.FrameDelayConverter;
    using MonitorRemoteViewPlugin.ViewModels;
    using NUnit.Framework;

    [TestFixture]
    class RemoteViewModelFactoryShould
    {
        private RemoteViewViewModelFactory _viewModelFactory;
        private IContinuousScreenshotController _controller;

        [SetUp]
        public void Setup()
        {
            _controller = A.Fake<IContinuousScreenshotController>();
            var controllerFactory = A.Fake<IContinuousScreenshotControllerFactory>();

            A.CallTo(() => controllerFactory.CreateController()).Returns(_controller);

            _viewModelFactory = new RemoteViewViewModelFactory(
                A.Fake<IScreenshotService>(),
                A.Fake<IScreenshotSavingUtility>(),
                A.Fake<ILoggerService>(),
                A.Fake<IConnectionService>(),
                A.Fake<ITaskSchedulerProvider>(),
                A.Fake<IEventService>(),
                A.Fake<IFrameDelayConverter>(),
                A.Fake<IMonitorDialog>(),
                controllerFactory);
        }

        [Test]
        public void CreateCorrectComponent()
        {
            var viewModel = _viewModelFactory.CreateComponent();

            Assert.That(viewModel, Is.TypeOf<RemoteViewViewModel>());
        }

        [Test]
        public void BeDecoratedWithExportAttribute()
        {
            Assert.IsTrue(Attribute.IsDefined(typeof(RemoteViewViewModelFactory), typeof(ExportAttribute)));
        }

        [Test]
        public void BeDecoratedWithContractTypeOfIDockableComponentFactory()
        {
            var attribute = (ExportAttribute)Attribute.GetCustomAttribute(
                typeof(RemoteViewViewModelFactory),
                typeof(ExportAttribute));

            Assert.That(attribute.ContractType.Name, Is.EqualTo(typeof(IDockableComponentFactory).Name));
        }

        [Test]
        public void UseTheCorrectInstanceOfTheScreenshotController()
        {
            var viewModel = (RemoteViewViewModel)_viewModelFactory.CreateComponent();

            viewModel.Dispose();

            A.CallTo(() => _controller.StopAsync(A<uint>.Ignored)).MustHaveHappened();
        }
    }
}
