////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Elektrobit Automotive GmbH
// Alle Rechte vorbehalten. All Rights Reserved.
// Information contained herein is subject to change without notice.
// Elektrobit retains ownership and all other rights in the software and each
// component thereof.
// Any reproduction of the software or components thereof without the prior
// written permission of Elektrobit is prohibited.
////////////////////////////////////////////////////////////////////////////////

namespace MonitorRemoteViewPlugin.Resources
{
    using System;
    using System.Collections.Generic;
    using System.ComponentModel.Composition;
    using Elektrobit.Guide.Ui.Tools;

    [Export(typeof(IResourceProvider))]
    class ExtensionResourceProvider : IResourceProvider
    {
        public IEnumerable<Uri> GetResourceUris()
        {
            return new Uri[]
                       {
                           new Uri(@"pack://application:,,,/MonitorRemoteViewPlugin;component/Resources/Resources.xaml")
                       };
        }
    }
}
