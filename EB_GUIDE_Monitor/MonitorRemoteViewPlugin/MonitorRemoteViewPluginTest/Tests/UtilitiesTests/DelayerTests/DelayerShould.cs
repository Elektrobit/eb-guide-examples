////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Elektrobit Automotive GmbH
// Alle Rechte vorbehalten. All Rights Reserved.
// Information contained herein is subject to change without notice.
// Elektrobit retains ownership and all other rights in the software and each
// component thereof.
// Any reproduction of the software or components thereof without the prior
// written permission of Elektrobit is prohibited.
////////////////////////////////////////////////////////////////////////////////

namespace MonitorRemoteViewPluginTest.Tests.UtilitiesTests.DelayerTests
{
    using System;
    using System.ComponentModel.Composition;
    using MonitorRemoteViewPlugin.Utilities.Delayer;
    using NUnit.Framework;

    [TestFixture]
    public class DelayerShould
    {
        [Test]
        public void BeDecoratedWithExportAttribute()
        {
            Assert.IsTrue(Attribute.IsDefined(typeof(Delayer), typeof(ExportAttribute)));
        }

        [Test]
        public void BeDecoratedWithContractTypeOfIDelayer()
        {
            var attribute = (ExportAttribute)Attribute.GetCustomAttribute(typeof(Delayer), typeof(ExportAttribute));

            Assert.That(attribute.ContractType.Name, Is.EqualTo(typeof(IDelayer).Name));
        }
    }
}