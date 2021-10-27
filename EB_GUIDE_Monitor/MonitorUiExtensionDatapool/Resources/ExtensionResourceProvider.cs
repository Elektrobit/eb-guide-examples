////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Elektrobit Automotive GmbH
// Alle Rechte vorbehalten. All Rights Reserved.
// Information contained herein is subject to change without notice.
// Elektrobit retains ownership and all other rights in the software and each
// component thereof.
// Any reproduction of the software or components thereof without the prior
// written permission of Elektrobit is prohibited.
////////////////////////////////////////////////////////////////////////////////

namespace MonitorUiExtension.Resources
{
    using System.Collections.Generic;
    using System.ComponentModel.Composition;

    using Elektrobit.Guide.Ui.Tools;

    [Export(typeof(IResourceProvider))]
    internal class ExtensionResourceProvider : IResourceProvider
    {
        public IEnumerable<ResourceLocation> GetResourceLocations()
        {
            yield return new ResourceLocation(
                @"pack://application:,,,/MonitorUiExtensionDatapool;component/Resources/Resources.xaml");
        }
    }
}
