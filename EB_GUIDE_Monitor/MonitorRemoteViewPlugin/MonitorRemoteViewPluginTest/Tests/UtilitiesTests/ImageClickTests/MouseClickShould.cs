////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Elektrobit Automotive GmbH
// Alle Rechte vorbehalten. All Rights Reserved.
// Information contained herein is subject to change without notice.
// Elektrobit retains ownership and all other rights in the software and each
// component thereof.
// Any reproduction of the software or components thereof without the prior
// written permission of Elektrobit is prohibited.
////////////////////////////////////////////////////////////////////////////////

namespace MonitorRemoteViewPluginTest.Tests.UtilitiesTests.ImageClickTests
{
    using System.Collections.Generic;
    using System.Linq;
    using Elektrobit.Guide.Monitor.Model.Event;
    using Elektrobit.Guide.Monitor.Model.Value;
    using Elektrobit.Guide.Monitor.Service.ScreenshotService;
    using FakeItEasy;
    using MonitorRemoteViewPlugin.ViewModels;
    using MonitorRemoteViewPluginTest.Tests.RemoteViewViewModelTests;
    using NUnit.Framework;

    [TestFixture]
    public class MouseClickShould : RemoteViewViewModelTestBase
    {
        [Test]
        public void CallFireEventFromIEventService()
        {
            var screenshot = A.Dummy<IScreenshot>();

            ViewModel.ScreenshotSource = screenshot.BitmapSource;

            A.CallTo(() => ContinuousScreenshotController.IsRunning).Returns(true);

            ViewModel.MouseClick(A.Dummy<double>(), A.Dummy<double>());

            A.CallTo(() => EventService.FireEvent(A<Event>.Ignored, A<IReadOnlyList<ModelValue>>.Ignored)).MustHaveHappened();
        }

        [Test]
        public void UseCorrectSceneId()
        {
            var screenshot = A.Dummy<IScreenshot>();

            ViewModel.ScreenshotSource = screenshot.BitmapSource;
            ViewModel.SelectedScene = 99;
            IReadOnlyList<ModelValue> customList = null;

            A.CallTo(() => EventService.FireEvent(A<Event>.Ignored, A<IReadOnlyList<ModelValue>>.Ignored)).Invokes((call) => customList = call.Arguments[1] as IReadOnlyList<ModelValue>);
            A.CallTo(() => ContinuousScreenshotController.IsRunning).Returns(true);

            ViewModel.MouseClick(A.Dummy<double>(), A.Dummy<double>());

            AssertThatEventParameterSceneIdIsCorrect(customList, 99);
        }

        private void AssertThatEventParameterSceneIdIsCorrect(IReadOnlyList<ModelValue> customList, int sceneId)
        {
            Assert.That(customList[0].Value, Is.EqualTo(sceneId));
        }

        [Test]
        public void SendTouchPressAndTouchReleaseEvents()
        {
            var screenshot = A.Dummy<IScreenshot>();
            var parameterLists = new List<List<object>>();

            ViewModel.ScreenshotSource = screenshot.BitmapSource;

            A.CallTo(() => ContinuousScreenshotController.IsRunning).Returns(true);

            A.CallTo(() => EventService.FireEvent(A<Event>.Ignored, A<IReadOnlyList<ModelValue>>.Ignored)).Invokes(
                (call) => parameterLists.Add(((IReadOnlyList<ModelValue>)call.Arguments[1]).Select(x => x.Value).ToList()));

            ViewModel.MouseClick(A.Dummy<double>(), A.Dummy<double>());

            Assert.That(parameterLists.Count, Is.EqualTo(2));

            AssertThatTouchTypeIsETouchPress(parameterLists[0]);
            AssertThatTouchTypeIsETouchRelease(parameterLists[1]);
        }

        private void AssertThatTouchTypeIsETouchPress(IList<object> eventParameters)
        {
            Assert.That(eventParameters[1], Is.EqualTo(RemoteViewViewModel.TOUCH_PRESS));
        }

        private void AssertThatTouchTypeIsETouchRelease(IList<object> eventParameters)
        {
            Assert.That(eventParameters[1], Is.EqualTo(RemoteViewViewModel.TOUCH_RELEASE));
        }

        [Test]
        public void SendsTheRightTouchTypeToEventService()
        {
            var screenshot = A.Dummy<IScreenshot>();

            ViewModel.ScreenshotSource = screenshot.BitmapSource;

            A.CallTo(() => ContinuousScreenshotController.IsRunning).Returns(true);

            ViewModel.MouseClick(A.Dummy<double>(), A.Dummy<double>());

            A.CallTo(() => EventService.FireEvent(A<Event>.That.Matches(touchEvent => touchEvent.GroupId == 11), A<IReadOnlyList<ModelValue>>.Ignored)).MustHaveHappened();
        }

        [Test]
        [TestCase(0.0, 0.0)]
        [TestCase(0.341, 0.6215)]
        [TestCase(0.5, 0.5)]
        [TestCase(0.9625, 0.292569)]
        [TestCase(1, 1)]
        public void SendScaledClickCoordinatesToGtf(double xCoordinate, double yCoordinate)
        {
            var screenshot = A.Fake<IScreenshot>();

            A.CallTo(() => screenshot.BitmapSource.Width).Returns(200);
            A.CallTo(() => screenshot.BitmapSource.Height).Returns(100);

            ViewModel.ScreenshotSource = screenshot.BitmapSource;

            A.CallTo(() => ContinuousScreenshotController.IsRunning).Returns(true);

            ViewModel.MouseClick(xCoordinate, yCoordinate);

            A.CallTo(
                () => EventService.FireEvent(A<Event>.Ignored,
                    A<IReadOnlyList<ModelValue>>.That.Matches(
                        listItem => listItem[2] == (int)(xCoordinate * ViewModel.ScreenshotSource.Width) && listItem[3] == (int)(yCoordinate * ViewModel.ScreenshotSource.Height)))).MustHaveHappened();
        }

        [Test]
        [TestCase(1.00001, 1.00000001)]
        [TestCase(5, 5)]
        [TestCase(-0.000001, -1.426)]
        [TestCase(-3, -3)]
        public void NotAllowOutOfBoundsCoordinates(double xCoordinate, double yCoordinate)
        {
            var screenshot = A.Dummy<IScreenshot>();

            ViewModel.ScreenshotSource = screenshot.BitmapSource;

            A.CallTo(() => ContinuousScreenshotRunner.IsRunning).Returns(true);

            ViewModel.MouseClick(xCoordinate, yCoordinate);

            A.CallTo(() => EventService.FireEvent(A<Event>.Ignored, A<IReadOnlyList<ModelValue>>.Ignored)).MustNotHaveHappened();
        }

        [Test]
        public void NotSendTouchEventsToGtfIfScreenshotSourceIsNull()
        {
            ViewModel.MouseClick(A.Dummy<double>(), A.Dummy<double>());

            A.CallTo(() => EventService.FireEvent(A<Event>.Ignored, A<IReadOnlyList<ModelValue>>.Ignored)).MustNotHaveHappened();
        }

        [Test]
        public void NotSendTouchEventsToGtfIfTheContinuousScreenshotRunnerIsNotRunning()
        {
            var screenshot = A.Dummy<IScreenshot>();

            ViewModel.ScreenshotSource = screenshot.BitmapSource;

            A.CallTo(() => ContinuousScreenshotRunner.IsRunning).Returns(false);

            ViewModel.MouseClick(A.Dummy<double>(), A.Dummy<double>());

            A.CallTo(() => EventService.FireEvent(A<Event>.Ignored, A<IReadOnlyList<ModelValue>>.Ignored))
                .MustNotHaveHappened();
        }
    }
}