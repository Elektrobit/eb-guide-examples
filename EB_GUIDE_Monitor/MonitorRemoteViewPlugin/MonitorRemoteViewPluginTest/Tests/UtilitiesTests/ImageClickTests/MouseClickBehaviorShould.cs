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
    using System.Threading;
    using System.Windows;
    using System.Windows.Controls;
    using System.Windows.Input;
    using FakeItEasy;
    using MonitorRemoteViewPlugin.Utilities.ImageClick;
    using MonitorRemoteViewPluginTest.Tests.RemoteViewViewModelTests;
    using NUnit.Framework;

    [TestFixture]
    public class MouseClickBehaviorShould : RemoteViewViewModelTestBase
    {
        [Test]
        [Apartment(ApartmentState.STA)]
        public void ConnectOnAttachToTheMouseClickHandler()
        {
            var mouseClickBehavior = new MouseClickBehavior();
            var image = new Image();
            var mouseClickHandler = A.Fake<IMouseClickHandler>();
            var mouseButtonEventArgs = new MouseButtonEventArgs(Mouse.PrimaryDevice, A.Dummy<int>(), MouseButton.Left)
                                           {
                                               RoutedEvent = UIElement.MouseLeftButtonDownEvent,
                                               Source = image
                                           };

            mouseClickBehavior.Attach(image);
            mouseClickBehavior.MouseClickHandler = mouseClickHandler;
            image.RaiseEvent(mouseButtonEventArgs);

            A.CallTo(() => mouseClickHandler.MouseClick(A<double>.Ignored, A<double>.Ignored)).MustHaveHappened();
        }


        [Test]
        [Apartment(ApartmentState.STA)]
        public void DisconnectOnDetachFromTheMouseClickHandler()
        {
            var mouseClickBehavior = new MouseClickBehavior();
            var image = new Image();
            var mouseClickHandler = A.Fake<IMouseClickHandler>();
            var mouseButtonEventArgs = new MouseButtonEventArgs(Mouse.PrimaryDevice, A.Dummy<int>(), MouseButton.Left)
                                           {
                                               RoutedEvent = UIElement.MouseLeftButtonDownEvent,
                                               Source = image
                                           };

            mouseClickBehavior.Attach(image);
            mouseClickBehavior.Detach();
            mouseClickBehavior.MouseClickHandler = mouseClickHandler;
            image.RaiseEvent(mouseButtonEventArgs);

            A.CallTo(() => mouseClickHandler.MouseClick(A<double>.Ignored, A<double>.Ignored)).MustNotHaveHappened();
        }
    }
}