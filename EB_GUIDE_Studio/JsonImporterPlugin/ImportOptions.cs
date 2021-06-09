////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Elektrobit Automotive GmbH
// Alle Rechte vorbehalten. All Rights Reserved.
// Information contained herein is subject to change without notice.
// Elektrobit retains ownership and all other rights in the software and each
// component thereof.
// Any reproduction of the software or components thereof without the prior
// written permission of Elektrobit is prohibited.
////////////////////////////////////////////////////////////////////////////////

namespace JsonImporterPlugin
{
    using Elektrobit.Guide.Options;

    /// <summary>
    /// Provides configuration items for importing Json files.
    /// </summary>
    /// <seealso cref="ImportMenuItemProvider"/>
    internal class ImportOptions : OptionsBase
    {
        public string ImportLocation { get; set; }

        public ImportOptions()
        {
            ImportLocation = string.Empty;
        }
    }
}
