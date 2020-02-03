namespace AnnotationsPlugin.Model
{
    using System.Collections.Generic;

    using Elektrobit.Guide.Studio.Model.Elements;
    using Elektrobit.Guide.Studio.Model.Session;
    using Elektrobit.Guide.Studio.TypeSystem;

    public class AnnotationPluginData : FixedTypeModelElement, IAnnotationPluginData
    {
        private readonly IModelPartChildListProperty<IAnnotationData> _annotations;

        [PropertyMapping(TypeMemberNames.ANNOTATIONS)]
        public IList<IAnnotationData> Annotations => _annotations.GetValue();

        public AnnotationPluginData()
        {
            _annotations = AddChildListProperty<IAnnotationData>(nameof(Annotations));
        }

        public override IType ResolveType(IProjectContext projectContext)
        {
            return LookupType<AnnotationPluginData>(projectContext, TypeNames.ANNOTATION_PLUGIN_DATA);
        }
    }
}