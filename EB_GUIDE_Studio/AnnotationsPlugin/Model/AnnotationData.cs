namespace AnnotationsPlugin.Model
{
    using Elektrobit.Guide.Studio.Model.Elements;
    using Elektrobit.Guide.Studio.Model.Session;
    using Elektrobit.Guide.Studio.TypeSystem;

    public class AnnotationData : FixedTypeModelElement, IAnnotationData
    {
        private readonly IModelPartReferenceProperty<IModelElement> _element;

        private readonly IModelPartValueProperty<string> _annotation;

        // The AnnotationData instance does not own the annotated element so the mapping mode must be set to Reference.
        [PropertyMapping(TypeMemberNames.ELEMENT, Mode = PropertyMappingAttribute.MappingMode.Reference)]
        public IModelElement Element
        {
            get => _element.GetValue();
            set => _element.SetValue(value);
        }

        // The PropertyMapping attribute maps element properties to type members registered by the type provider.
        [PropertyMapping(TypeMemberNames.ANNOTATION)]
        public string Annotation
        {
            get => _annotation.GetValue();
            set => _annotation.SetValue(value);
        }

        public AnnotationData()
        {
            _element = AddReferenceProperty<IModelElement>(nameof(Element));
            _annotation = AddStringProperty(nameof(Annotation));
        }

        // This method is used to retrieve the type registered by the type provider from the element.
        public override IType ResolveType(IProjectContext projectContext)
        {
            // LookupType uses an internal caching mechanism for fast type lookup.
            // The generic type argument will be used as key for the lookup operation.
            return LookupType<AnnotationData>(projectContext, TypeNames.ANNOTATION_DATA);
        }
    }
}