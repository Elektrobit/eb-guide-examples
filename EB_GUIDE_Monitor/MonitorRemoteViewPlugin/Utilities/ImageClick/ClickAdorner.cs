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
    using System;
    using System.Windows;
    using System.Windows.Documents;
    using System.Windows.Media;
    using System.Windows.Media.Animation;

    public class ClickAdorner : Adorner
    {
        private readonly Point _clickPoint;

        public static readonly DependencyProperty RadiusProperty = DependencyProperty.Register(
            nameof(Radius), 
            typeof(double), 
            typeof(ClickAdorner), 
            new FrameworkPropertyMetadata(0.0d, FrameworkPropertyMetadataOptions.AffectsRender));

        public double Radius
        {
            get
            {
                return (double)GetValue(RadiusProperty);
            }
            set
            {
                SetValue(RadiusProperty, value);
            }
        }

        public ClickAdorner(UIElement adornedElement, Point clickPoint)
            : base(adornedElement)
        {
            _clickPoint = clickPoint;
            Loaded += AnimAdornerLoaded;
        }

        protected override void OnRender(DrawingContext drawingContext)
        {
            Pen pen = new Pen(new SolidColorBrush(Colors.LawnGreen), 2);
            drawingContext.DrawEllipse(null, pen, _clickPoint, Radius, Radius);
        }

        void AnimAdornerLoaded(object sender, RoutedEventArgs e)
        {
            DoubleAnimation doubleAnimation = new DoubleAnimation
                                                    {
                                                        From = 1.0,
                                                        To = 15.0,
                                                        Duration = new Duration(TimeSpan.FromMilliseconds(400)),
                                                    };

            Storyboard storyboard = new Storyboard();
            storyboard.Completed += StoryboardOnCompleted;
            storyboard.Children.Add(doubleAnimation);
            Storyboard.SetTarget(storyboard, this);
            Storyboard.SetTargetProperty(doubleAnimation, new PropertyPath(RadiusProperty));

            storyboard.Begin(this);
        }

        private void StoryboardOnCompleted(object sender, EventArgs e)
        {
            DeactivateAdorner();
        }

        public void ActivateAdorner()
        {
            var adornerLayer = AdornerLayer.GetAdornerLayer(AdornedElement);
            adornerLayer?.Add(this);
        }

        private void DeactivateAdorner()
        {
            var adornerLayer = AdornerLayer.GetAdornerLayer(AdornedElement);
            adornerLayer?.Remove(this);
        }
    }
}