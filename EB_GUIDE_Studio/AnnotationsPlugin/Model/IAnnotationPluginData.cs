namespace AnnotationsPlugin.Model
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
