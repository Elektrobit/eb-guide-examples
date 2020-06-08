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


namespace AnnotationsServicePlugin.Model
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