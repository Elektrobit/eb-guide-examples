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
    using System;
    using System.ComponentModel;
    using System.ComponentModel.Composition;
    using System.Threading.Tasks;
    using Elektrobit.Guide.Monitor.Service.ScreenshotService;
    using FakeItEasy;
    using MonitorRemoteViewPlugin.Utilities.ContinuousScreenshot;
    using NUnit.Framework;

    [TestFixture]
    public class ContinuousScreenshotControllerShould
    {
        private const uint SCENE_ID = 5;

        [Test]
        public async Task CallRunFromScreenshotRunnerWithToggle()
        {
            var screenshotRunner = A.Fake<IContinuousScreenshotRunner>();

            var screenshotController = new ContinuousScreenshotController(screenshotRunner);

            await screenshotController.ToggleAsync(SCENE_ID);

            A.CallTo(() => screenshotRunner.Run(SCENE_ID)).MustHaveHappened();
        }

        [Test]
        public async Task NotCallStopFromScreenshotRunnerWithOnlyOneToggle()
        {
            var screenshotRunner = A.Dummy<IContinuousScreenshotRunner>();
            var screenshotController = new ContinuousScreenshotController(screenshotRunner);

            await screenshotController.ToggleAsync(SCENE_ID);

            A.CallTo(() => screenshotRunner.Stop()).MustNotHaveHappened();
        }

        [Test]
        public async Task CallStopFromScreenshotRunnerWithTwoToggles()
        {
            var screenshotRunner = A.Fake<IContinuousScreenshotRunner>();
            A.CallTo(() => screenshotRunner.IsRunning).ReturnsNextFromSequence(false, false, true);

            var screenshotController = new ContinuousScreenshotController(screenshotRunner);

            await screenshotController.ToggleAsync(SCENE_ID);
            await screenshotController.ToggleAsync(SCENE_ID);

            A.CallTo(() => screenshotRunner.Stop()).MustHaveHappened();

        }

        [Test]
        public async Task CallRunFromScreenshotRunnerOnlyOnceWhenToggledTwice()
        {
            var screenshotRunner = A.Fake<IContinuousScreenshotRunner>();

            A.CallTo(() => screenshotRunner.IsRunning).ReturnsNextFromSequence(false, false, true, false, false);

            var screenshotController = new ContinuousScreenshotController(screenshotRunner);

            await screenshotController.ToggleAsync(SCENE_ID);
            await screenshotController.ToggleAsync(SCENE_ID);

            A.CallTo(() => screenshotRunner.Run(SCENE_ID)).MustHaveHappened(Repeated.Exactly.Once);
        }

        [Test]
        public async Task CallRunFromScreenshotRunnerTwiceWhenToggledThreeTimes()
        {
            var screenshotRunner = A.Fake<IContinuousScreenshotRunner>();

            A.CallTo(() => screenshotRunner.IsRunning).ReturnsNextFromSequence(false, false, true, false, false);

            var screenshotController = new ContinuousScreenshotController(screenshotRunner);

            await screenshotController.ToggleAsync(SCENE_ID);
            await screenshotController.ToggleAsync(SCENE_ID);
            await screenshotController.ToggleAsync(SCENE_ID);

            A.CallTo(() => screenshotRunner.Run(SCENE_ID)).MustHaveHappened(Repeated.Exactly.Twice);
        }

        [Test]
        public async Task CallStopFromScreenshotRunnerWithStop()
        {
            var screenshotRunner = A.Dummy<IContinuousScreenshotRunner>();
            var screenshotController = new ContinuousScreenshotController(screenshotRunner);

            await screenshotController.StartAsync(SCENE_ID);
            await screenshotController.StopAsync(SCENE_ID);

            A.CallTo(() => screenshotRunner.Stop()).MustHaveHappened();
        }

        [Test]
        public void ThrowExceptionForMissingScreenshotRunner()
        {
            Assert.That(() => new ContinuousScreenshotController(null), Throws.ArgumentNullException.With.Property("ParamName").EqualTo("continuousScreenshotRunner"));
        }

        [Test]
        public async Task PassesTheCorrectSceneIdToTheScreenshotRunner()
        {
            var screenshotRunner = A.Fake<IContinuousScreenshotRunner>();
            var screenshotController = new ContinuousScreenshotController(screenshotRunner);

            await screenshotController.StartAsync(SCENE_ID);

            A.CallTo(() => screenshotRunner.Run(SCENE_ID)).MustHaveHappened();
        }

        [Test]
        public void ThrowAnExceptionWhenStartedAndRunnerIsAlreadyRunning()
        {
            var screenshotRunner = A.Fake<IContinuousScreenshotRunner>();
            A.CallTo(() => screenshotRunner.IsRunning).Returns(true);

            var screenshotController = new ContinuousScreenshotController(screenshotRunner);

            Assert.That(async () => await screenshotController.StartAsync(SCENE_ID), Throws.Exception);
        }

        [Test]
        public async Task AllowCallingStartAndStopWithTheSameSceneId()
        {
            var screenshotRunner = A.Fake<IContinuousScreenshotRunner>();

            var screenshotController = new ContinuousScreenshotController(screenshotRunner);

            await screenshotController.StartAsync(SCENE_ID);

            Assert.That(() => screenshotController.StopAsync(SCENE_ID), Throws.Nothing);
        }

        [Test]
        public async Task NotAllowCallingStopWithDifferentSceneIdsAfterRunningStart()
        {
            var screenshotRunner = A.Fake<IContinuousScreenshotRunner>();

            var screenshotController = new ContinuousScreenshotController(screenshotRunner);

            await screenshotController.StartAsync(SCENE_ID);

            Assert.That(() => screenshotController.StopAsync(SCENE_ID + 1), Throws.Exception);
        }

        // ReSharper disable once NUnit.MethodWithParametersAndTestAttribute
        [Test]
        public async Task PassSceneIdToScreenshotRunnerWithToggle([Random(1, 10, 5)] int signedSceneId)
        {
            var screenshotRunner = A.Fake<IContinuousScreenshotRunner>();

            var sceneId = (uint)signedSceneId;

            var screenshotController = new ContinuousScreenshotController(screenshotRunner);

            await screenshotController.ToggleAsync(sceneId);

            A.CallTo(() => screenshotRunner.Run(sceneId)).MustHaveHappened();
        }

        [Test]
        public void GetScreenshotFromScreenshotRunnerWhenScreenshotPropertyHasChanged()
        {
            var screenShot = A.Fake<IScreenshot>();
            var screenshotRunner = A.Fake<IContinuousScreenshotRunner>();
            A.CallTo(() => screenshotRunner.Screenshot).Returns(screenShot);
            var screenshotController = new ContinuousScreenshotController(screenshotRunner);

            screenshotRunner.PropertyChanged += Raise.FreeForm.With(screenshotRunner, new PropertyChangedEventArgs("Screenshot"));

            Assert.That(screenshotController.Screenshot, Is.SameAs(screenShot));
        }

        [Test]
        public void BeDecoratedWithExportAttribute()
        {
            Assert.IsTrue(Attribute.IsDefined(typeof(ContinuousScreenshotController), typeof(ExportAttribute)));
        }

        [Test]
        public void BeDecoratedWithContractTypeOfIContinuousScreenshotTaker()
        {
            var attribute = (ExportAttribute)Attribute.GetCustomAttribute(
                typeof(ContinuousScreenshotController),
                typeof(ExportAttribute));

            Assert.That(attribute.ContractType.Name, Is.EqualTo(typeof(IContinuousScreenshotController).Name));
        }

        // ReSharper disable once NUnit.MethodWithParametersAndTestAttribute
        [Test]
        public void PropagateDelayChangesToScreenshotRunner([Random(0, 100, 5)] int signedDelay)
        {
            var delay = (uint)signedDelay;
            var screenshotRunner = A.Fake<IContinuousScreenshotRunner>();
            var screenshotController = new ContinuousScreenshotController(screenshotRunner) { Delay = delay };

            Assert.That(screenshotRunner.Delay, Is.EqualTo(delay));
        }

        // ReSharper disable once NUnit.MethodWithParametersAndTestAttribute
        [Test]
        public void PropagateDelayChangesFromScreenshotRunner([Random(0, 100, 5)] int signedDelay)
        {
            var delay = (uint)signedDelay;
            var screenshotRunner = A.Fake<IContinuousScreenshotRunner>();
            var screenshotController = new ContinuousScreenshotController(screenshotRunner);
            screenshotRunner.Delay = delay;

            Assert.That(screenshotController.Delay, Is.EqualTo(delay));
        }

        [Test]
        public void RegisterAtScreenshotRunnerDuringConstruction()
        {
            var screenshotRunner = A.Fake<IContinuousScreenshotRunner>();
            var screenshotController = new ContinuousScreenshotController(screenshotRunner);

            AssertEventHandlerAdded(screenshotRunner, nameof(INotifyPropertyChanged.PropertyChanged));
        }

        [Test]
        public void DeregisterFromScreenshotRunnerDuringDisposal()
        {
            var screenshotRunner = A.Fake<IContinuousScreenshotRunner>();
            var screenshotController = new ContinuousScreenshotController(screenshotRunner);

            screenshotController.Dispose();

            AssertEventHandlerRemoved(screenshotRunner, nameof(INotifyPropertyChanged.PropertyChanged));
        }

        private static void AssertEventHandlerAdded(object obj, string eventName)
        {
            AssertMethodWasCalled(obj, $"add_{eventName}");
        }

        private static void AssertMethodWasCalled(object obj, string methodName)
        {
            A.CallTo(obj).Where(x => x.Method.Name.Equals(methodName)).MustHaveHappened();
        }

        private static void AssertEventHandlerRemoved(object obj, string eventName)
        {
            AssertMethodWasCalled(obj, $"remove_{eventName}");
        }

        [Test]
        public void NotGetScreenshotFromScreenshotRunnerIfThePropertyNameIsNotScreenshot()
        {
            var screenshotRunner = A.Fake<IContinuousScreenshotRunner>();
            var screenshotController = new ContinuousScreenshotController(screenshotRunner);

            screenshotRunner.PropertyChanged += Raise.FreeForm.With(screenshotRunner, new PropertyChangedEventArgs("some property"));

            A.CallTo(() => screenshotRunner.Screenshot).MustNotHaveHappened();
        }

        [Test]
        public void NotGetScreenshotFromScreenshotRunnerWhenTheEventSenderIsNotTheScreenshotRunner()
        {
            var screenshotRunner = A.Fake<IContinuousScreenshotRunner>();
            var screenshotController = new ContinuousScreenshotController(screenshotRunner);

            screenshotRunner.PropertyChanged += Raise.FreeForm.With(this, new PropertyChangedEventArgs("Screenshot"));

            A.CallTo(() => screenshotRunner.Screenshot).MustNotHaveHappened();
        }

        [Test]
        [TestCase(true)]
        [TestCase(false)]
        [TestCase(true)]
        public void GetLoopStatusFromScreenshotRunner(bool status)
        {
            var screenshotRunner = A.Fake<IContinuousScreenshotRunner>();
            var screenshotController = new ContinuousScreenshotController(screenshotRunner);

            A.CallTo(() => screenshotRunner.IsRunning).Returns(status);

            Assert.That(screenshotController.IsRunning, Is.EqualTo(status));
        }
    }
}
