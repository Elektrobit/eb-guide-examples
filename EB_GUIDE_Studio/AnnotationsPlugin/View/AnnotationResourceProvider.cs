namespace AnnotationsPlugin.View
{
    using System;
    using System.Collections.Generic;
    using System.ComponentModel.Composition;

    using Elektrobit.Guide.Ui.Tools;

    /// <summary>
    /// IResourceProvider implementation that is used to tell WPF where to look for resources provided by this plugin.
    /// </summary>
    [Export(typeof(IResourceProvider))]
    internal class AnnotationResourceProvider : IResourceProvider
    {
        public IEnumerable<Uri> GetResourceUris()
        {
            yield return new Uri(@"AnnotationsPlugin;Component/View/Resources.xaml", UriKind.Relative);
        }
    }
}
