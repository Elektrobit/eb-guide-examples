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
    using System.ComponentModel.Composition;
    using Elektrobit.Guide.Monitor.Service.ScreenshotService;
    using MonitorRemoteViewPlugin.Utilities.ContinuousScreenshot;
    using NUnit.Framework;
    using FakeItEasy;
    using MonitorRemoteViewPlugin.Utilities.Delayer;

    [TestFixture]
    public class ContinuousScreenshotControllerFactoryShould
    {
        [Test]
        public void CreateAScreenshotController()
        {
            var fakeScreenshotService = A.Fake<IScreenshotService>();
            var fakeDelayer = A.Fake<IDelayer>();

            var realFactory = new ContinuousScreenshotControllerFactory(fakeScreenshotService, fakeDelayer);

            var controllerInstance = realFactory.CreateController();

            Assert.That(controllerInstance, Is.InstanceOf<IContinuousScreenshotController>());
        }

        [Test]
        public void BeDecoratedWithExportAttribute()
        {
            Assert.IsTrue(Attribute.IsDefined(typeof(ContinuousScreenshotControllerFactory), typeof(ExportAttribute)));
        }

        [Test]
        public void BeDecoratedWithContractTypeOfIContinuousScreenshotControllerFactory()
        {
            var attribute = (ExportAttribute)Attribute.GetCustomAttribute(
                typeof(ContinuousScreenshotControllerFactory),
                typeof(ExportAttribute));

            Assert.That(attribute.ContractType.Name, Is.EqualTo(typeof(IContinuousScreenshotControllerFactory).Name));
        }
    }
}