namespace AnnotationsPlugin.Model
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