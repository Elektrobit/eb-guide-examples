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
    using System;
    using System.Collections.Generic;
    using System.ComponentModel.Composition;

    using AnnotationsServicePlugin.Service;

    using Elektrobit.Guide.Studio.Model.Actions;
    using Elektrobit.Guide.Studio.Model.Elements;
    using Elektrobit.Guide.Studio.Model.Session;

    /// <summary>
    /// A ICustomDeleteAction is needed when additional elements shall be removed from the model when some elements are deleted.
    /// In our case we want to delete the annotation if an annotated element is deleted.
    /// </summary>
    [Export(typeof(ICustomDeleteAction))]
    internal class AnnotationDeleter : ICustomDeleteAction
    {
        private readonly Lazy<IAnnotationService> _annotationService;

        // Lazy import here to avoid cyclic imports
        // Rule of thumb: When importing services in the service or model layer
        // use lazy imports to avoid import cycles.
        [ImportingConstructor]
        public AnnotationDeleter(Lazy<IAnnotationService> annotationService)
        {
            _annotationService = annotationService;
        }

        public void Delete(
            IWriteSession session,
            IProjectContext projectContext,
            IReadOnlyCollection<IModelElement> deletedElements)
        {

            foreach (var element in deletedElements)
            {
                _annotationService.Value.DeleteAnnotation(session, projectContext, element);
            }
        }
    }
}
