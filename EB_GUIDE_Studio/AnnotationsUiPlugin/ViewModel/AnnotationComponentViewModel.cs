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

    using AnnotationsServicePlugin.Service;

    using Elektrobit.Guide.Scheduler;
    using Elektrobit.Guide.Studio.Model.Elements;
    using Elektrobit.Guide.Studio.Workbench;
    using Elektrobit.Guide.Studio.Workbench.Selection;
    using Elektrobit.Guide.Studio.Workbench.ViewModels;

    /// <summary>
    /// View model for the annotation docking component.
    /// </summary>
    internal class AnnotationComponentViewModel : DockablePartViewModelBase
    {
        private readonly IWorkbenchViewModel _workbench;

        private readonly IAnnotationService _annotationService;

        private readonly ITaskSchedulerProvider _schedulerProvider;

        private object _current;

        /// <summary>
        /// The currently displayed content of the component.
        /// Can be a anything: A complex view model or a simple string.
        /// </summary>
        public object Current
        {
            get => _current;

            private set
            {
                var oldValue = _current;
                if (SetProperty(ref _current, value))
                {
                    (oldValue as IDisposable)?.Dispose();
                }
            }
        }

        public AnnotationComponentViewModel(
            WorkbenchPartId contentId,
            IWorkbenchViewModel workbench,
            IAnnotationService annotationService,
            ITaskSchedulerProvider schedulerProvider)
            : base(contentId)
        {
            _workbench = workbench;
            _annotationService = annotationService;
            _schedulerProvider = schedulerProvider;

            // Title of the component as shown in the component's tab.
            // You may use a translatable string here.
            Title = "Annotations";

            // We want to get notified about selection changes
            // so we have to register the ISelectionConsumer service
            RegisterService<ISelectionConsumer>(new SelectionConsumer(OnSelectionChanged));

            // Initialize the component with the current selection of the workbench
            UpdateSelection(_workbench.CurrentSelection);
        }

        protected override void Dispose(bool disposing)
        {
            if (disposing)
            {
                Current = null;
            }

            base.Dispose(disposing);
        }

        private void OnSelectionChanged(SelectionSource source, ModelSelection selection)
        {
            UpdateSelection(selection);
        }

        private void UpdateSelection(ModelSelection selection)
        {
            // We need a selected element ...
            if (selection.Count == 0)
            {
                Current = "No selection";
                return;
            }

            // ... but don't support multi-selection.
            if (selection.Count > 1)
            {
                Current = "Multi-selection is not supported";
                return;
            }

            var element = selection[0].GetNearest<IModelElement>();

            var viewModel = new AnnotationViewModel(
                                element,
                                _workbench.ProjectContext,
                                _annotationService,
                                _schedulerProvider)
                                { Parent = this };
            Current = viewModel;
        }
    }
}
