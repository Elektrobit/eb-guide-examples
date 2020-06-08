////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Elektrobit Automotive GmbH
// Alle Rechte vorbehalten. All Rights Reserved.
// Information contained herein is subject to change without notice.
// Elektrobit retains ownership and all other rights in the software and each
// component thereof.
// Any reproduction of the software or components thereof without the prior
// written permission of Elektrobit is prohibited.
////////////////////////////////////////////////////////////////////////////////

namespace MonitorUiExtension.ViewModels
{
    using Elektrobit.Guide.Options;

    /// <summary>
    /// Provides configuration items for <see cref="ExtensionComponentViewModel"/>.
    /// </summary>
    internal class ExtensionComponentOptions : OptionsBase
    {
        public string SelectedScene { get; set; }

        public ExtensionComponentOptions()
        {
            SelectedScene = string.Empty;
        }
    }
}
