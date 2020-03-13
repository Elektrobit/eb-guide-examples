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

    /// <summary>
    /// Top level element for the annotation plugin data.
    /// Holds a list of annotations.
    /// </summary>
    public interface IAnnotationPluginData : IModelElement
    {
        /// <summary>
        /// The list of stored annotations.
        /// List properties are defined as getter-only properties.
        /// </summary>
        IList<IAnnotationData> Annotations { get; }
    }
}
