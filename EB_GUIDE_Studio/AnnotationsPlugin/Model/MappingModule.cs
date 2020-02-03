namespace AnnotationsPlugin.Model
{
    using System.ComponentModel.Composition;

    using Elektrobit.Guide.Module;
    using Elektrobit.Guide.Studio.Model.Session;

    using Prism.Mef.Modularity;

    /// <summary>
    /// This module registers the mappings for our custom elements when EB GUIDE Studio initializes its modules.
    /// The mappings are needed to transfer data between the model and the internal data storage.
    /// </summary>
    [ModuleExport(typeof(MappingModule))]
    internal class MappingModule : GuideModule
    {
        private readonly IModelElementMapper _modelElementMapper;

        [ImportingConstructor]
        public MappingModule(IApplicationLifecycleManager lifecycleManager, IModelElementMapper modelElementMapper)
            : base(lifecycleManager)
        {
            _modelElementMapper = modelElementMapper;
        }

        public override void Initialize()
        {
            RegisterMappings();
        }

        private void RegisterMappings()
        {
            _modelElementMapper.RegisterAnnotated<AnnotationPluginData>(TypeNames.ANNOTATION_PLUGIN_DATA);
            _modelElementMapper.RegisterAnnotated<AnnotationData>(TypeNames.ANNOTATION_DATA);
        }
    }
}
