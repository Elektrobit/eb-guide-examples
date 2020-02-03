namespace AnnotationsPlugin.Model
{
    using System.ComponentModel.Composition;
    
    using Elektrobit.Guide.Studio.StandardTypes;
    using Elektrobit.Guide.Studio.StandardTypes.ScalarTypes;
    using Elektrobit.Guide.Studio.StandardTypes.TypeProvider;
    using Elektrobit.Guide.Studio.TypeSystem;
    
    /// <summary>
    /// A type provider registers type descriptors at the type manager.
    /// A type descriptor describes how data is stored internally and in the storage format on the disk.
    /// </summary>
    [Export(typeof(ITypeProvider))]
    internal class AnnotationTypeProvider : TypeProviderBase
    {
        // Just some constants to shorten the type names when used
        private const string OBJECT = Elektrobit.Guide.Studio.StandardTypes.TypeNames.OBJECT;
        private const string STRING = Elektrobit.Guide.Studio.StandardTypes.TypeNames.STRING;

        public AnnotationTypeProvider()
        {
            // ANNOTATION_DATA_LIST is a list of ANNOTATION_DATA instances.
            AddDescription(
                new ListTypeDescription<ElementListType>
                    {
                        TypeName = TypeNames.ANNOTATION_DATA_LIST,
                        BaseTypeName = OBJECT,
                        ElementTypeName = TypeNames.ANNOTATION_DATA
                    });

            // ANNOTATION_PLUGIN_DATA holds a ANNOTATION_DATA_LIST.
            AddDescription(
                new StructureTypeDescription<DefaultStructureType>
                    {
                        TypeName = TypeNames.ANNOTATION_PLUGIN_DATA,
                        BaseTypeName = OBJECT,
                        Members = GetMembers(
                            ValueTypeMember(TypeMemberNames.ANNOTATIONS, TypeNames.ANNOTATION_DATA_LIST))
                    });

            // ANNOTATION_DATA holds an ELEMENT member and an ANNOTATIONS member.
            AddDescription(
                new StructureTypeDescription<DefaultStructureType>
                    {
                        TypeName = TypeNames.ANNOTATION_DATA,
                        BaseTypeName = OBJECT,
                        Members = GetMembers(
                            ValueTypeMember(TypeMemberNames.ELEMENT, ObjectReferenceType(OBJECT)),
                            ValueTypeMember(TypeMemberNames.ANNOTATION, STRING))
                    });
        }
    }
}
