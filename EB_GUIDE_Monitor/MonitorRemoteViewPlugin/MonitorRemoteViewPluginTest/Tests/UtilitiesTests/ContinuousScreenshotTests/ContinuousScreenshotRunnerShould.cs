////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Elektrobit Automotive GmbH
// Alle Rechte vorbehalten. All Rights Reserved.
// Information contained herein is subject to change without notice.
// Elektrobit retains ownership and all other rights in the software and each
// component thereof.
// Any reproduction of the software or components thereof without the prior
// written permission of Elektrobit is prohibited.
////////////////////////////////////////////////////////////////////////////////

namespace MonitorRemoteViewPluginTest.Tests.UtilitiesTests.ContinuousScreenshotTests
{
    using System.ComponentModel;
    using System.Threading;
    using System.Threading.Tasks;
    using Elektrobit.Guide.Monitor.Service.ScreenshotService;
    using FakeItEasy;
    using MonitorRemoteViewPlugin.Utilities.Cancellation;
    using MonitorRemoteViewPlugin.Utilities.ContinuousScreenshot;
    using MonitorRemoteViewPlugin.Utilities.Delayer;
    using NUnit.Framework;

    [TestFixture]
    public class ContinuousScreenshotRunnerShould
    {
        private const uint SCENE_ID = 5;

        // ReSharper disable once NUnit.MethodWithParametersAndTestAttribute
        [Test]
        public async Task CallDelayer([Random(50, 100, 5)] int signedDelay)
        {
            var delay = (uint)signedDelay;
            var delayer = A.Fake<IDelayer>();

            var cancellation = A.Fake<ICancellation>();
            A.CallTo(() => cancellation.IsRunning).ReturnsNextFromSequence(true, false);

            var screenshotRunner =
                new ContinuousScreenshotRunner(cancellation, A.Dummy<IScreenshotService>(), delayer) { Delay = delay };

            await screenshotRunner.Run(4);

            A.CallTo(() => delayer.FrameDelay(delay, cancellation)).MustHaveHappened();
        }

        // ReSharper disable once NUnit.MethodWithParametersAndTestAttribute
        [Test]
        public async Task CallsScreenshotServiceWhileItsRunning([Random(2, 20, 5)] int numberOfScreenShots)
        {
            var screenshotService = A.Fake<IScreenshotService>();

            var cancellation = A.Fake<ICancellation>();
            A.CallTo(() => cancellation.IsRunning).Returns(true).NumberOfTimes(numberOfScreenShots).Then.Returns(false);

            var screenshotRunner = new ContinuousScreenshotRunner(cancellation, screenshotService, A.Dummy<IDelayer>());

            await screenshotRunner.Run(SCENE_ID);

            A.CallTo(() => screenshotService.GetScreenshotAsync(SCENE_ID)).MustHaveHappened(Repeated.Exactly.Times(numberOfScreenShots));
        }

        // ReSharper disable once NUnit.MethodWithParametersAndTestAttribute
        [Test]
        public async Task PassTheRightSceneIdWhenTakingScreenshots([Random(2, 20, 5)] int signedSceneId)
        {
            var sceneId = (uint)signedSceneId;
            var screenshotService = A.Fake<IScreenshotService>();

            var cancellation = A.Fake<ICancellation>();
            A.CallTo(() => cancellation.IsRunning).ReturnsNextFromSequence(true, false);

            var screenshotRunner = new ContinuousScreenshotRunner(cancellation, screenshotService, A.Dummy<IDelayer>());

            await screenshotRunner.Run(sceneId);

            A.CallTo(() => screenshotService.GetScreenshotAsync(sceneId)).MustHaveHappened();
        }

        [Test]
        public async Task AssignTakenScreenshotsToTheScreenshotProperty()
        {
            var screenShot = A.Dummy<IScreenshot>();
            var screenshotService = A.Fake<IScreenshotService>();

            var cancellation = A.Fake<ICancellation>();
            A.CallTo(() => cancellation.IsRunning).ReturnsNextFromSequence(true, false);

            var tokenSource = new CancellationTokenSource();
            A.CallTo(() => screenshotService.GetScreenshotAsync(A<uint>.Ignored)).
                Invokes(() => tokenSource.Cancel()).Returns(Task.FromResult(screenShot));

            var screenshotRunner = new ContinuousScreenshotRunner(cancellation, screenshotService, A.Dummy<IDelayer>());

            await screenshotRunner.Run(SCENE_ID);

            Assert.That(screenshotRunner.Screenshot, Is.SameAs(screenShot));
        }

        [Test]
        public async Task CallCancellationStopWhenStopped()
        {
            var cancellation = A.Fake<ICancellation>();

            var screenshotRunner = new ContinuousScreenshotRunner(cancellation, A.Dummy<IScreenshotService>(), A.Dummy<IDelayer>());

            await screenshotRunner.Stop();

            A.CallTo(() => cancellation.Cancel()).MustHaveHappened();
        }

        [Test]
        public void ThrowExceptionForMissingCancellation()
        {
            Assert.That(() => new ContinuousScreenshotRunner(null, A.Dummy<IScreenshotService>(), A.Dummy<IDelayer>()), Throws.ArgumentNullException.With.Property("ParamName").EqualTo("cancellation"));
        }

        [Test]
        public void ThrowExceptionForMissingDelayer()
        {
            Assert.That(() => new ContinuousScreenshotRunner(A.Dummy<ICancellation>(), A.Dummy<IScreenshotService>(), null), Throws.ArgumentNullException.With.Property("ParamName").EqualTo("delayer"));
        }

        [Test]
        public void ThrowExceptionForMissingScreenshotService()
        {
            Assert.That(() => new ContinuousScreenshotRunner(A.Dummy<ICancellation>(), null, A.Dummy<IDelayer>()), Throws.ArgumentNullException.With.Property("ParamName").EqualTo("screenshotService"));
        }

        [Test]
        public async Task StartCancellationWhenStarted()
        {
            var cancellation = A.Fake<ICancellation>();

            var screenshotRunner = new ContinuousScreenshotRunner(cancellation, A.Dummy<IScreenshotService>(), A.Dummy<IDelayer>());

            await screenshotRunner.Run(SCENE_ID);

            A.CallTo(() => cancellation.Reset()).MustHaveHappened();
        }

        [Test]
        public async Task RaisePropertyChangedEventForScreenshotWhenScreenshotIsTaken()
        {
            var screenShot = A.Dummy<IScreenshot>();

            var screenshotService = A.Fake<IScreenshotService>();

            var cancellation = A.Fake<ICancellation>();
            A.CallTo(() => cancellation.IsRunning).ReturnsNextFromSequence(true, false);

            var tokenSource = new CancellationTokenSource();
            A.CallTo(() => screenshotService.GetScreenshotAsync(A<uint>.Ignored)).
                Invokes(() => tokenSource.Cancel()).Returns(Task.FromResult(screenShot));

            var screenshotRunner = new ContinuousScreenshotRunner(cancellation, screenshotService, A.Dummy<IDelayer>());

            string changedProperty = string.Empty;
            void ScreenshotRunnerOnPropertyChanged(object sender, PropertyChangedEventArgs args) => changedProperty = args.PropertyName;
            screenshotRunner.PropertyChanged += ScreenshotRunnerOnPropertyChanged;

            await screenshotRunner.Run(SCENE_ID);
            screenshotRunner.PropertyChanged -= ScreenshotRunnerOnPropertyChanged;

            Assert.That(changedProperty, Is.SameAs("Screenshot"));
        }
    }
}
