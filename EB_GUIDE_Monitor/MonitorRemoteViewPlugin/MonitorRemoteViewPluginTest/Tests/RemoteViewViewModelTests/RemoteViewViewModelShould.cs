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
    using System.Collections.Generic;
    using System.ComponentModel;
    using System.Linq;
    using System.Threading;
    using System.Threading.Tasks;
    using System.Windows.Media;
    using System.Windows.Media.Imaging;
    using Elektrobit.Guide.Monitor.Connection;
    using Elektrobit.Guide.Monitor.Service;
    using Elektrobit.Guide.Monitor.Service.ScreenshotService;
    using Elektrobit.Guide.Monitor.Ui.Docking;
    using FakeItEasy;
    using FluentAssertions;
    using MonitorRemoteViewPlugin.Utilities.ImageClick;
    using Elektrobit.Guide.Monitor.Connection.Protocols;
    using NUnit.Framework;
    using Elektrobit.Guide.Monitor.Messages.Events;
    using Elektrobit.Guide.Monitor.Ui.Dialog;
    using Elektrobit.Guide.Scheduler;

    using MonitorRemoteViewPlugin.ViewModels;

    [TestFixture]
    class RemoteViewViewModelShould : RemoteViewViewModelTestBase
    {
        [Test]
        public void BeInstanceOfIDockableComponentViewModel()
        {
            Assert.That(ViewModel, Is.InstanceOf<IDockableComponentViewModel>());
        }

        [Test]
        public void UpdateTheScreenshotSourceWhenRefreshIsExecuted()
        {
            var screenshot = A.Dummy<IScreenshot>();

            A.CallTo(() => ScreenshotService.GetScreenshotAsync(ViewModel.SelectedScene)).Returns(screenshot);

            ViewModel.Refresh.Execute();

            Assert.That(ViewModel.ScreenshotSource, Is.EqualTo(screenshot.BitmapSource));
        }

        [Test]
        [TestCase((uint)0)]
        [TestCase((uint)1)]
        [TestCase((uint)2)]
        [TestCase((uint)11)]
        [TestCase((uint)134)]
        public void CallGetScreenshotAsyncFromIScreenshotServiceWhenRefreshIsExecuted(uint sceneId)
        {
            ViewModel.SelectedScene = sceneId;

            ViewModel.Refresh.Execute();

            A.CallTo(() => ScreenshotService.GetScreenshotAsync(sceneId)).MustHaveHappened();
        }

        [Test]
        public void DisableSaveExecutionIfScreenshotSourceIsNull()
        {
            Assert.That(ViewModel.Save.CanExecute, Is.False);
        }

        [Test]
        public void EnableSaveExecutionIfSceenshotSourceIsSet()
        {
            var screenshot = A.Dummy<IScreenshot>();

            ViewModel.ScreenshotSource = screenshot.BitmapSource;

            Assert.That(ViewModel.Save.CanExecute, Is.True);
        }

        [Test]
        public void NotCallSaveScreenshotFromBitmapSourceIfScreenshotSourceIsNull()
        {
            A.CallTo(() => ScreenshotSavingUtility.SaveScreenshotFromBitmapSource(ViewModel.ScreenshotSource, A<string>.Ignored)).MustNotHaveHappened();
        }

        [Test]
        public void CallSaveScreenshotFromBitmapSourceIfScreenshotSourceIsValid()
        {
            var screenshot = A.Dummy<IScreenshot>();

            ViewModel.ScreenshotSource = screenshot.BitmapSource;

            A.CallTo(() => MonitorDialog.SaveFileDialog(A<string>.Ignored, A<string>.Ignored, A<string>.Ignored))
                .Returns(new FileSelectionResult() { Success = true });

            ViewModel.Save.Execute();

            A.CallTo(() => ScreenshotSavingUtility.SaveScreenshotFromBitmapSource(ViewModel.ScreenshotSource, A<string>.Ignored)).MustHaveHappened();
        }

        [Test]
        public void TellTheUserThatTheScreenshotHasBeenSaved()
        {
            A.CallTo(() => MonitorDialog.SaveFileDialog(A<string>.Ignored, A<string>.Ignored, A<string>.Ignored))
                .Returns(new FileSelectionResult() { Success = true });

            ViewModel.Save.Execute();

            A.CallTo(() => LoggerService.LogMessage(ViewModel, "Screenshot saved.", Severity.Info)).MustHaveHappened();
        }

        [Test]
        public void NotTellTheUserThatTheScreenshotWasSavedIfTheUserDoesNotClickOkInTheSaveFileDialog()
        {
            A.CallTo(() => MonitorDialog.SaveFileDialog(A<string>.Ignored, A<string>.Ignored, A<string>.Ignored))
                .Returns(new FileSelectionResult() { Success = false });

            ViewModel.Save.Execute();

            A.CallTo(() => LoggerService.LogMessage(ViewModel, "Screenshot saved.", Severity.Info)).MustNotHaveHappened();
        }

        [Test]
        public void ShowLogErrorMessageWhenSaveScreenshotFromBitmapSourceThrowsException()
        {
            A.CallTo(() => MonitorDialog.SaveFileDialog(A<string>.Ignored, A<string>.Ignored, A<string>.Ignored))
                .Returns(new FileSelectionResult() { Success = true });

            A.CallTo(() => ScreenshotSavingUtility.SaveScreenshotFromBitmapSource(A<BitmapSource>.Ignored, A<string>.Ignored)).Throws<Exception>();

            ViewModel.Save.Execute();

            A.CallTo(() => LoggerService.LogMessage(ViewModel, A<string>.Ignored, Severity.Error)).MustHaveHappened();
        }

        [Test]
        public void DisableRefreshExecutionWhenTheMonitorIsNotConnectedToAGtfModel()
        {
            A.CallTo(() => ConnectionService.IsConnected).Returns(false);

            Assert.That(ViewModel.Refresh.CanExecute(), Is.False);
        }

        [Test]
        public void EnableRefreshExecutionWhenTheMonitorIsConnectedToAGtfModel()
        {
            A.CallTo(() => ConnectionService.IsConnected).Returns(true);

            Assert.That(ViewModel.Refresh.CanExecute(), Is.True);
        }

        [Test]
        public void DetectWhenAnImageIsShownAndEnableSaveExecution()
        {
            var screenshot = A.Dummy<IScreenshot>();

            ViewModel.Save.MonitorEvents();

            ViewModel.ScreenshotSource = screenshot.BitmapSource;

            ViewModel.Save.ShouldRaise(nameof(ViewModel.Save.CanExecuteChanged));
        }

        [Test]
        public void DetectWhenMonitorIsConnectedToTheModelAndEnableRefreshExecution()
        {
            ViewModel.Refresh.MonitorEvents();

            ConnectionService.Connected += Raise.WithEmpty();

            ViewModel.Refresh.ShouldRaise(nameof(ViewModel.Refresh.CanExecuteChanged));
        }

        [Test]
        public void DetectWhenMonitorIsDisconnectedFromTheModelAndDisableRefreshExecution()
        {
            ViewModel.Refresh.MonitorEvents();

            ConnectionService.Disconnected += Raise.WithEmpty();

            ViewModel.Refresh.ShouldRaise(nameof(ViewModel.Refresh.CanExecuteChanged));
        }

        [Test]
        public void DetectWhenMonitorIsConnectedToTheModelAndEnableAutoRefreshExecution()
        {
            ViewModel.AutoRefresh.MonitorEvents();

            ConnectionService.Connected += Raise.WithEmpty();

            ViewModel.AutoRefresh.ShouldRaise(nameof(ViewModel.AutoRefresh.CanExecuteChanged));
        }

        [Test]
        public void DetectWhenMonitorIsDisconnectedFromTheModelAndDisableAutoRefreshExecution()
        {
            ViewModel.AutoRefresh.MonitorEvents();

            ConnectionService.Disconnected += Raise.WithEmpty();

            ViewModel.AutoRefresh.ShouldRaise(nameof(ViewModel.AutoRefresh.CanExecuteChanged));
        }

        [Test]
        public void ImplementIMouseClickHandler()
        {
            Assert.That(ViewModel, Is.InstanceOf<IMouseClickHandler>());
        }

        [Test]
        public void PassFramesPerSecondToTheFrameDelayConverter()
        {
            ViewModel.FramesPerSecond = 50;

            A.CallTo(() => FrameDelayConverter.Convert(50)).MustHaveHappened();
        }

        [Test]
        public void SendAMessageToGtf()
        {
            ConnectionService.Connected += Raise.WithEmpty();

            A.CallTo(() => ConnectionService.SendAsync(A<IMessage>.Ignored)).MustHaveHappened();
        }

        [Test]
        public void SendAMessageToSubscribeToGtfEvents()
        {
            ConnectionService.Connected += Raise.WithEmpty();

            A.CallTo(
                () => ConnectionService.SendAsync(
                    A<SubscribeEventGroups>.That.Matches(
                        x => x.Subscribe == true))).MustHaveHappened();
        }

        [Test]
        public void SendAMessageToGtfToSubscribeToTheCorrectEvents()
        {
            ConnectionService.Connected += Raise.WithEmpty();

            A.CallTo(
                    () => ConnectionService.SendAsync(
                        A<SubscribeEventGroups>.That.Matches(
                            x => x.GroupIds.SequenceEqual(EventGroups.TouchEvents)))).MustHaveHappened();
        }

        [Test]
        public void SubscribeToGtfTouchEventsDuringConstructionIfTheMonitorIsAlreadyConnectedToTheModel()
        {
            A.CallTo(() => ConnectionService.IsConnected).Returns(true);

            new RemoteViewViewModel(ScreenshotService, ScreenshotSavingUtility, LoggerService, ConnectionService, SchedulerProvider, EventService, ContinuousScreenshotController, FrameDelayConverter, MonitorDialog);

            A.CallTo(() => ConnectionService.SendAsync(
                A<SubscribeEventGroups>.That.Matches(
                    x => x.Subscribe == true &&
                    x.GroupIds.All(y => EventGroups.TouchEvents.Contains(y))))).MustHaveHappened();
        }

        [Test]
        [TestCaseSource(nameof(GetTouchEventGroups))]
        public void ShowOnlyFireEventsFromGtf(uint groupId)
        {
            var message = new FireEvent { GroupId = groupId };

            ConnectionService.MessageReceived += Raise.With(this, new MessageReceivedEventArgs(message));

            A.CallTo(
                    () => LoggerService.LogMessage(
                        ViewModel,
                        groupId.ToString(),
                        Severity.Info))
                .MustHaveHappened();
        }

        private static IEnumerable<uint> GetTouchEventGroups()
        {
            return EventGroups.TouchEvents;
        }

        [Test]
        public void PassTheDelayDuringConstruction()
        {
            A.CallTo(() => FrameDelayConverter.Convert(A<uint>.Ignored)).Returns((uint)532);

            new RemoteViewViewModel(ScreenshotService, ScreenshotSavingUtility, LoggerService, ConnectionService, SchedulerProvider, EventService, ContinuousScreenshotController, FrameDelayConverter, MonitorDialog);

            A.CallToSet(() => ContinuousScreenshotController.Delay).To(532).MustHaveHappened();
        }

        [Test]
        public void SetOneFramePerSecondAsDefaultValue()
        {
            Assert.That(ViewModel.FramesPerSecond, Is.EqualTo(1));
        }

        [Test]
        public void StopTheProductionOfInvisibleScreenshotsWhenThePluginIsClosedWithAutoRefreshStillRunning()
        {
            ViewModel.Dispose();

            A.CallTo(() => ContinuousScreenshotController.StopAsync(ViewModel.SelectedScene)).MustHaveHappened();
        }

        [Test]
        public void GetScreenshotBitmapFromContinuousScreenshotTakerWhenScreenshotPropertyHasChanged()
        {
            var source = new WriteableBitmap(1, 1, 96, 96, PixelFormats.Pbgra32, null);

            A.CallTo(() => ContinuousScreenshotController.Screenshot.BitmapSource).Returns(source);

            ContinuousScreenshotController.PropertyChanged += Raise.FreeForm.With(ContinuousScreenshotController, new PropertyChangedEventArgs("Screenshot"));

            Assert.That(ViewModel.ScreenshotSource, Is.SameAs(source));
        }

        [Test]
        public void NotGetScreenshotBitmapFromContinuousScreenshotTakerIfThePropertyNameIsNotScreenshot()
        {
            ContinuousScreenshotController.PropertyChanged += Raise.FreeForm.With(ContinuousScreenshotController, new PropertyChangedEventArgs("any other property"));

            A.CallTo(() => ContinuousScreenshotController.Screenshot.BitmapSource).MustNotHaveHappened();
        }

        [Test]
        public void NotGetScreenshotBitmapFromContinuousScreenshotTakerWhenTheEventSenderIsNotTheContinuousScreenshotTaker()
        {
            ContinuousScreenshotController.PropertyChanged += Raise.FreeForm.With(this, new PropertyChangedEventArgs("Screenshot"));

            A.CallTo(() => ContinuousScreenshotController.Screenshot.BitmapSource).MustNotHaveHappened();
        }

        [Test]
        public void LetTheUserDecideWhereToSaveTheImage()
        {
            ViewModel.Save.Execute();

            A.CallTo(() => MonitorDialog.SaveFileDialog(A<string>.Ignored, A<string>.Ignored, A<string>.Ignored))
                .MustHaveHappened();
        }

        [Test]
        public void GiveAnAppropriateTitleToTheSaveFileWindow()
        {
            ViewModel.Save.Execute();

            A.CallTo(() => MonitorDialog.SaveFileDialog("Save Image As", A<string>.Ignored, A<string>.Ignored))
                .MustHaveHappened();
        }

        [Test]
        public void GiveAnAppropriateDefaultNameToTheScreenshot()
        {
            ViewModel.Save.Execute();

            A.CallTo(() => MonitorDialog.SaveFileDialog(A<string>.Ignored, "Screenshot", A<string>.Ignored))
                .MustHaveHappened();
        }

        [Test]
        public void SetTheFiletypeFilterToPngImages()
        {
            ViewModel.Save.Execute();

            A.CallTo(
                () => MonitorDialog.SaveFileDialog(
                    A<string>.Ignored,
                    A<string>.Ignored,
                    " PNG image file (*.png) | *.png")).MustHaveHappened();
        }

        [Test]
        public void SaveTheScreenshotWhenTheUserClicksOkInTheSaveFileDialog()
        {
            A.CallTo(() => MonitorDialog.SaveFileDialog(A<string>.Ignored, A<string>.Ignored, A<string>.Ignored))
                .Returns(new FileSelectionResult() { Success = true });

            ViewModel.Save.Execute();

            A.CallTo(
                () => ScreenshotSavingUtility.SaveScreenshotFromBitmapSource(
                    A<BitmapSource>.Ignored,
                    A<string>.Ignored)).MustHaveHappened();
        }

        [Test]
        public void SaveTheCorrectImageWhenTheUserClicksOkInTheSaveFileDialog()
        {
            var screenshot = A.Dummy<IScreenshot>();

            ViewModel.ScreenshotSource = screenshot.BitmapSource;

            A.CallTo(() => MonitorDialog.SaveFileDialog(A<string>.Ignored, A<string>.Ignored, A<string>.Ignored))
                .Returns(new FileSelectionResult() { Success = true });

            ViewModel.Save.Execute();

            A.CallTo(() => ScreenshotSavingUtility.SaveScreenshotFromBitmapSource(screenshot.BitmapSource, A<string>.Ignored)).MustHaveHappened();
        }

        [Test]
        public void NotSaveTheScreenshotIfTheUserDidNotClickOkInTheSaveFileDialog()
        {
            A.CallTo(() => MonitorDialog.SaveFileDialog(A<string>.Ignored, A<string>.Ignored, A<string>.Ignored))
                .Returns(new FileSelectionResult() { Success = false });

            ViewModel.Save.Execute();

            A.CallTo(() => ScreenshotSavingUtility.SaveScreenshotFromBitmapSource(A<BitmapSource>.Ignored, A<string>.Ignored)).MustNotHaveHappened();
        }

        [Test]
        public void SaveTheScreenshotAtThePathSelectedByTheUser()
        {
            var path = "Any path";

            A.CallTo(() => MonitorDialog.SaveFileDialog(A<string>.Ignored, A<string>.Ignored, A<string>.Ignored))
                .Returns(new FileSelectionResult() { Success = true, FileName = path });

            ViewModel.Save.Execute();

            A.CallTo(() => ScreenshotSavingUtility.SaveScreenshotFromBitmapSource(A<BitmapSource>.Ignored, path)).MustHaveHappened();
        }

        [Test]
        public void ShowALogErrorMessageWhenSaveFileDialogThrowsAnException()
        {
            A.CallTo(() => MonitorDialog.SaveFileDialog(A<string>.Ignored, A<string>.Ignored, A<string>.Ignored))
                .Returns(new FileSelectionResult() { Success = true });

            A.CallTo(() => MonitorDialog.SaveFileDialog(A<string>.Ignored, A<string>.Ignored, A<string>.Ignored))
                .Throws<Exception>();

            ViewModel.Save.Execute();

            A.CallTo(() => LoggerService.LogMessage(ViewModel, A<string>.Ignored, Severity.Error)).MustHaveHappened();
        }

        [Test]
        public void UpdateTheScreenshotSourceViaTheUiDispatcherWhenExecutingSave()
        {
            A.CallTo(() => MonitorDialog.SaveFileDialog(A<string>.Ignored, A<string>.Ignored, A<string>.Ignored))
                .Returns(new FileSelectionResult() { Success = true });

            ViewModel.Save.Execute();

            A.CallTo(
                () => SchedulerProvider.MainScheduler.InvokeAsync(
                    A<Func<Task>>.Ignored,
                    A<CancellationToken>.Ignored,
                    A<ExecutionMode>.Ignored)).MustHaveHappened();
        }
    }
}
