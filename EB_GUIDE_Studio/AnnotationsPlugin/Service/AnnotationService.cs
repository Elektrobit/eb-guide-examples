namespace AnnotationsPlugin.Service
{
    using System;
    using System.ComponentModel.Composition;
    using System.Linq;
    using System.Reactive;
    using System.Reactive.Linq;

    using AnnotationsPlugin.Model;

    using Elektrobit.Guide.Studio.Model.Actions;
    using Elektrobit.Guide.Studio.Model.Actions.Plugins;
    using Elektrobit.Guide.Studio.Model.Elements;
    using Elektrobit.Guide.Studio.Model.Reactive;
    using Elektrobit.Guide.Studio.Model.Session;

    using ReactiveUI;

    [Export(typeof(IAnnotationService))]
    internal class AnnotationService : IAnnotationService
    {
        private const string PLUGIN_ID = "AnnotationPlugin";

        private readonly IPluginDataService _pluginDataService;

        private readonly IModelElementService _modelElementService;

        [ImportingConstructor]
        public AnnotationService(
            IPluginDataService pluginDataService,
            IModelElementService modelElementService)
        {
            _pluginDataService = pluginDataService;
            _modelElementService = modelElementService;
        }

        public IAnnotationData GetAnnotation(IProjectContext projectContext, IModelElement element)
        {
            return GetAnnotationData(projectContext, element);
        }

        public IAnnotationData SetAnnotation(IWriteSession session, IProjectContext projectContext, IModelElement element, string annotation)
        {
            var pluginData = GetPluginData(projectContext);
            if (pluginData == null)
            {
                pluginData = new AnnotationPluginData();
                _pluginDataService.SetPluginData(session, projectContext, PLUGIN_ID, pluginData);
            }

            var annotationData = GetAnnotationData(projectContext, element);
            if (annotationData == null)
            {
                annotationData = new AnnotationData { Element = element, Annotation = annotation };

                pluginData.Annotations.Add(annotationData);
                session.Save(pluginData);
            }
            else
            {
                annotationData.Annotation = annotation;
            }

            session.Save(annotationData);

            return annotationData;
        }

        public void DeleteAnnotation(IWriteSession session, IProjectContext projectContext, IModelElement element)
        {
            var annotationData = GetAnnotationData(projectContext, element);
            if (annotationData == null)
            {
                return;
            }

            _modelElementService.DeleteElements(session, projectContext, annotationData);
        }

        private IAnnotationPluginData GetPluginData(IProjectContext projectContext)
        {
            return _pluginDataService.GetPluginData(projectContext, PLUGIN_ID) as IAnnotationPluginData;
        }

        private IAnnotationData GetAnnotationData(IProjectContext projectContext, IModelElement element)
        {
            var pluginData = GetPluginData(projectContext);
            return pluginData?.Annotations.SingleOrDefault(ad => Equals(ad.Element, element));
        }

        public IObservable<string> ObserveAnnotation(IProjectContext projectContext, IModelElement element)
        {
            // Local helper method that returns the model part from the event args.
            // Returns null if the element has been removed from the model.
            T PartFromChange<T>(EventPattern<ModelChangedEventArgs> pattern)
                where T : IModelPart
            {
                return pattern.EventArgs.Action == ModelChangedAction.Remove
                           ? default(T)
                           : (T)pattern.EventArgs.Part;
            }

            // Local helper method that observes the annotation data entries.
            // If there is no IAnnotationPluginData yet an empty string is returned by the observable.
            IObservable<string> ObserveAnnotationData(IAnnotationPluginData data)
            {
                if (data == null)
                {
                    return Observable.Return(string.Empty);
                }

                return data.OnModelChanged()
                    .WhereAddOrRemove<IAnnotationData>()
                    .Where(pattern => Equals(((IAnnotationData)pattern.EventArgs.Part).Element, element))
                    .Select(PartFromChange<IAnnotationData>)
                    .StartWith(GetAnnotationData(projectContext, element))
                    .Select(ObserveAnnotation)
                    .Switch();
            }

            // Local helper method that observes the Annotation property.
            // If the annotation is null an empty string is returned by the observable.
            IObservable<string> ObserveAnnotation(IAnnotationData annotation)
            {
                if (annotation == null)
                {
                    return Observable.Return(string.Empty);
                }

                return annotation.WhenAnyValue(x => x.Annotation);
            }

            return projectContext.Project
                .OnModelChanged()
                .WhereAddOrRemove<IAnnotationPluginData>()
                .Select(PartFromChange<IAnnotationPluginData>)
                .StartWith(GetPluginData(projectContext))
                .Select(ObserveAnnotationData)
                .Switch();
        }
    }
}