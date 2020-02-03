////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Elektrobit Automotive GmbH
// Alle Rechte vorbehalten. All Rights Reserved.
// Information contained herein is subject to change without notice.
// Elektrobit retains ownership and all other rights in the software and each
// component thereof.
// Any reproduction of the software or components thereof without the prior
// written permission of Elektrobit is prohibited.
////////////////////////////////////////////////////////////////////////////////

namespace MonitorRemoteViewPluginTest.Tests.UtilitiesTests.FrameDelayConverterTests
{
    using System;
    using System.ComponentModel.Composition;
    using MonitorRemoteViewPlugin.Utilities.FrameDelayConverter;
    using NUnit.Framework;

    [TestFixture]
    public class FrameDelayConverterShould
    {
        [Test]
        public void ReturnFullSecondForInvalidArgumentWhenConstructedWithNoDefaultDelay()
        {
            var converter = new FrameDelayConverter();

            Assert.That(converter.Convert(0), Is.EqualTo(1000));
        }

        [Test]
        [TestCase((uint)500)]
        [TestCase((uint)300)]
        public void ReturnDefaultDelayForInvalidArgumentWhenConstructedWithDefaultDelay(uint defaultDelay)
        {
            var converter = new FrameDelayConverter(defaultDelay);

            Assert.That(converter.Convert(0), Is.EqualTo(defaultDelay));
        }

        [Test]
        [TestCase((uint)25, ExpectedResult = (uint)40)]
        [TestCase((uint)30, ExpectedResult = (uint)33)]
        [TestCase((uint)50, ExpectedResult = (uint)20)]
        [TestCase((uint)60, ExpectedResult = (uint)16)]
        public uint ConvertFramesPerSecondToFrameDelay(uint framesPerSecond)
        {
            var converter = new FrameDelayConverter();

            return converter.Convert(framesPerSecond);
        }

        [Test]
        public void BeDecoratedWithExportAttribute()
        {
            Assert.IsTrue(Attribute.IsDefined(typeof(FrameDelayConverter), typeof(ExportAttribute)));
        }

        [Test]
        public void BeDecoratedWithContractTypeOfIFrameDelayConverter()
        {
            var attribute = (ExportAttribute)Attribute.GetCustomAttribute(
                typeof(FrameDelayConverter),
                typeof(ExportAttribute));

            Assert.That(attribute.ContractType.Name, Is.EqualTo(typeof(IFrameDelayConverter).Name));
        }
    }
}
