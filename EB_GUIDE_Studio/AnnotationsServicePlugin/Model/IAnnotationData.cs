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
    using Elektrobit.Guide.Studio.Model.Elements;

    /// <summary>
    /// A annotation for a model element.
    /// </summary>
    public interface IAnnotationData : IModelElement
    {
        /// <summary>
        /// Gets or sets the annotated element.
        /// </summary>
        IModelElement Element { get; set; }

        /// <summary>
        /// Gets or sets the annotation string.
        /// </summary>
        string Annotation { get; set; }
    }
}