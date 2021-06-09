////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Elektrobit Automotive GmbH
// Alle Rechte vorbehalten. All Rights Reserved.
// Information contained herein is subject to change without notice.
// Elektrobit retains ownership and all other rights in the software and each
// component thereof.
// Any reproduction of the software or components thereof without the prior
// written permission of Elektrobit is prohibited.
////////////////////////////////////////////////////////////////////////////////

namespace MonitorRemoteViewPlugin.Utilities.ImageClick
{
    using System.Windows;
    using System.Windows.Controls;
    using System.Windows.Input;
    using System.Windows.Interactivity;

    public class MouseClickBehavior : Behavior<Image>
    {
        public static readonly DependencyProperty MouseClickHandlerProperty = DependencyProperty.Register(
            nameof(MouseClickHandler),
            typeof(IMouseClickHandler),
            typeof(MouseClickBehavior),
            new PropertyMetadata(null));

        public IMouseClickHandler MouseClickHandler
        {
            get { return (IMouseClickHandler)GetValue(MouseClickHandlerProperty); }
            set { SetValue(MouseClickHandlerProperty, value); }
        }

        protected override void OnAttached()
        {
            base.OnAttached();
            AssociatedObject.MouseLeftButtonDown += OnMouseLeftButtonDown;
        }

        protected override void OnDetaching()
        {
            AssociatedObject.MouseLeftButtonDown -= OnMouseLeftButtonDown;
            base.OnDetaching();
        }

        private void OnMouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            var position = e.GetPosition(AssociatedObject);
            var adorner = new ClickAdorner(AssociatedObject, position);

            adorner.ActivateAdorner();
            MouseClickHandler.MouseClick(position.X / AssociatedObject.ActualWidth, position.Y / AssociatedObject.ActualHeight);
        }
    }
}