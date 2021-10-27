////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Elektrobit Automotive GmbH
// Alle Rechte vorbehalten. All Rights Reserved.
//
// Information contained herein is subject to change without notice.
// Elektrobit retains ownership and all other rights in the software and each
// component thereof.
// Any reproduction of the software or components thereof without the prior
// written permission of Elektrobit is prohibited.
////////////////////////////////////////////////////////////////////////////////


namespace AnnotationsUiPlugin.ViewModel
{
    using System;
    using System.ComponentModel.Composition;

    using AnnotationsServicePlugin.Service;

    using Elektrobit.Guide.Scheduler;
    using Elektrobit.Guide.Studio.Workbench;
    using Elektrobit.Guide.Studio.Workbench.ViewModels;
    using Elektrobit.Guide.Ui.Docking.ViewModels;

    /// <summary>
    /// A factory class that is used to create the items in the Layout menu and to create
    /// the annotation component when added by the user.
    /// </summary>
    [Export(typeof(IDockablePartFactory))]
    internal class AnnotationComponentFactory : DockablePartFactoryBase
    {
        private readonly Lazy<IAnnotationService> _annotationService;

        private readonly Lazy<ITaskSchedulerProvider> _schedulerProvider;

        /// <summary>
        /// Title as shown in the Layout menu.
        /// You may use a translatable string here.
        /// </summary>
        public override string Title => "Annotations";

        // Lazy import of the services because we don't really need them in this class.
        // We will pass them along to the view model when creating it.
        [ImportingConstructor]
        public AnnotationComponentFactory(
            Lazy<IAnnotationService> annotationService,
            Lazy<ITaskSchedulerProvider> schedulerProvider)
            : base("plugin_annotations") // Unique id of the component type. Don't translate this string.
        {
            _annotationService = annotationService;
            _schedulerProvider = schedulerProvider;
        }

        protected override DockablePartViewModelBase CreatePartImpl(
            IWorkbenchViewModel workbench,
            WorkbenchPartId contentId)
        {
            return new AnnotationComponentViewModel(
                contentId,
                workbench,
                _annotationService.Value,
                _schedulerProvider.Value);
        }
    }
}
