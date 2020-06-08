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
    using System.Reactive.Disposables;
    using System.Reactive.Linq;
    using System.Threading.Tasks;
    using System.Windows.Input;

    using AnnotationsServicePlugin.Service;

    using Elektrobit.Guide.Reactive;
    using Elektrobit.Guide.Scheduler;
    using Elektrobit.Guide.Studio.Model.Actions;
    using Elektrobit.Guide.Studio.Model.Elements;
    using Elektrobit.Guide.Ui.Mvvm;

    using ReactiveUI;

    /// <summary>
    /// View model that allows editing of a element's annotation.
    /// </summary>
    internal class AnnotationViewModel : ViewModel, IDisposable
    {
        private readonly IModelElement _element;

        private readonly IProjectContext _projectContext;

        private readonly IAnnotationService _annotationService;

        private readonly ITaskSchedulerProvider _schedulerProvider;

        private readonly CompositeDisposable _compositeDisposable;

        private string _storedAnnotationText;

        private string _annotationText;

        /// <summary>
        /// The annotation text as stored in the model.
        /// </summary>
        private string StoredAnnotationText
        {
            get => _storedAnnotationText;
            set => SetProperty(ref _storedAnnotationText, value);
        }

        /// <summary>
        /// The annotation text as shown in the UI.
        /// Two-way bound to a TextBox.
        /// </summary>
        public string AnnotationText
        {
            get => _annotationText;
            set => SetProperty(ref _annotationText, value);
        }

        /// <summary>
        /// The name of the element as shown in the UI.
        /// </summary>
        public string ElementName { get; }

        /// <summary>
        /// The command to store the annotation text from the UI in the model.
        /// </summary>
        public ICommand SaveAnnotationCommand { get; }

        public AnnotationViewModel(
            IModelElement element,
            IProjectContext projectContext,
            IAnnotationService annotationService,
            ITaskSchedulerProvider schedulerProvider)
        {
            _element = element;
            _projectContext = projectContext;
            _annotationService = annotationService;
            _schedulerProvider = schedulerProvider;
            _compositeDisposable = new CompositeDisposable();

            ElementName = _element.ToElementName(_projectContext);

            // Synchronizes the model value with the view model
            _annotationService.ObserveAnnotation(projectContext, element)
                .ObserveOn(schedulerProvider.MainScheduler)
                .Subscribe(a =>
                    {
                        StoredAnnotationText = a;
                        AnnotationText = a;
                    })
                .DisposeWith(_compositeDisposable);

            // Save is only enabled, if the text in the UI is different from the stored text
            var canSave = this.WhenAnyValue(x => x.AnnotationText, x => x.StoredAnnotationText)
                .Select(_ => AnnotationText != StoredAnnotationText);
            
            SaveAnnotationCommand = ReactiveCommand.CreateFromTask(SaveAnnotation, canSave);
        }

        public void Dispose()
        {
            _compositeDisposable.Dispose();
        }

        private async Task SaveAnnotation()
        {
            var annotationText = AnnotationText;
            
            // The annotation text is empty -> delete the annotation if present
            if (string.IsNullOrWhiteSpace(annotationText))
            {
                if (_annotationService.GetAnnotation(_projectContext, _element) == null)
                {
                    return;
                }

                await _schedulerProvider.ExecuteModelAction(
                    _projectContext,
                    session => _annotationService.DeleteAnnotation(session, _projectContext, _element));

                return;
            }

            // otherwise, save the annotation
            await _schedulerProvider.ExecuteModelAction(
                _projectContext,
                session => _annotationService.SetAnnotation(session, _projectContext, _element, annotationText));
        }
    }
}