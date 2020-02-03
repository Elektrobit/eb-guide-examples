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
    using System.Threading;
    using System.Threading.Tasks;

    using Elektrobit.Guide.Scheduler;

    using FakeItEasy;
    using NUnit.Framework;

    [TestFixture]
    class AutoRefreshShould : RemoteViewViewModelTestBase
    {
        [Test]
        public void DisableAutoRefreshExecutionWhenTheMonitorIsNotConnectedToTheModel()
        {
            A.CallTo(() => ConnectionService.IsConnected).Returns(false);

            Assert.That(ViewModel.AutoRefresh.CanExecute, Is.False);
        }

        [Test]
        public void EnableAutoRefreshExecutionWhenTheMonitorIsConnectedToTheModel()
        {
            A.CallTo(() => ConnectionService.IsConnected).Returns(true);

            Assert.That(ViewModel.AutoRefresh.CanExecute, Is.True);
        }

        [Test]
        public async Task CallUiDispatcherInvokeWhenAutoRefreshIsExecuted()
        {
            await ViewModel.AutoRefresh.Execute();

            A.CallTo(
                () => SchedulerProvider.MainScheduler.InvokeAsync(
                    A<Func<Task>>.Ignored,
                    A<CancellationToken>.Ignored,
                    A<ExecutionMode>.Ignored)).MustHaveHappened();
        }

        [Test]
        [TestCase((uint)3)]
        [TestCase((uint)7)]
        [TestCase((uint)10)]
        public async Task HaveTheUiDispatcherCallGetScreenshotAsyncFromScreenshotService(uint sceneNumber)
        {
            Func<Task> capturedAction = null;
            ViewModel.SelectedScene = sceneNumber;
            A.CallTo(
                () => SchedulerProvider.MainScheduler.InvokeAsync(
                    A<Func<Task>>.Ignored,
                    A<CancellationToken>.Ignored,
                    A<ExecutionMode>.Ignored)).Invokes(call => capturedAction = (Func<Task>)call.Arguments[0]);

            await ViewModel.AutoRefresh.Execute();
            await capturedAction();

            var sceneId = sceneNumber;
            A.CallTo(() => ContinuousScreenshotController.ToggleAsync(sceneId)).MustHaveHappened();
        }
    }
}
