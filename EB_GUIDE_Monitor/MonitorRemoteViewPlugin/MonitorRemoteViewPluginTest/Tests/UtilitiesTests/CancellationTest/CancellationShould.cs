////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Elektrobit Automotive GmbH
// Alle Rechte vorbehalten. All Rights Reserved.
// Information contained herein is subject to change without notice.
// Elektrobit retains ownership and all other rights in the software and each
// component thereof.
// Any reproduction of the software or components thereof without the prior
// written permission of Elektrobit is prohibited.
////////////////////////////////////////////////////////////////////////////////

namespace MonitorRemoteViewPluginTest.Tests.UtilitiesTests.CancellationTest
{
    using MonitorRemoteViewPlugin.Utilities.Cancellation;
    using NUnit.Framework;

    [TestFixture]
    public class CancellationShould
    {
        [Test]
        public void BeStoppedByDefault()
        {
            var cancellation = new Cancellation();

            Assert.That(cancellation.IsRunning, Is.False);
        }

        [Test]
        public void HaveNoCancellationTokenSourceByDefault()
        {
            var cancellation = new Cancellation();

            Assert.That(cancellation.Source, Is.Null);
        }

        [Test]
        public void BeRunningWhenStarted()
        {
            var cancellation = new Cancellation();

            cancellation.Reset();

            Assert.That(cancellation.IsRunning, Is.True);
        }

        [Test]
        public void HaveACancellationTokenWhenStarted()
        {
            var cancellation = new Cancellation();

            cancellation.Reset();

            Assert.That(cancellation.Source, Is.Not.Null);
        }

        [Test]
        public void HaveSameCancellationTokenWhenStartedTwice()
        {
            var cancellation = new Cancellation();

            cancellation.Reset();
            var expectedToken = cancellation.Source;
            cancellation.Reset();

            Assert.That(cancellation.Source, Is.SameAs(expectedToken));
        }

        [Test]
        public void HaveNewCancellationTokenWhenStartedTwiceAndStoppedInBetween()
        {
            var cancellation = new Cancellation();

            cancellation.Reset();
            var expectedToken = cancellation.Source;
            cancellation.Cancel();
            cancellation.Reset();

            Assert.That(cancellation.Source, Is.Not.SameAs(expectedToken));
        }

        [Test]
        public void BeStoppedWhenStopped()
        {
            var cancellation = new Cancellation();

            cancellation.Reset();
            cancellation.Cancel();

            Assert.That(cancellation.IsRunning, Is.False);
        }

        [Test]
        public void BeStoppedWhenSourceIsCanceled()
        {
            var cancellation = new Cancellation();

            cancellation.Reset();
            var source = cancellation.Source;
            source.Cancel();

            Assert.That(cancellation.IsRunning, Is.False);
        }
    }
}
