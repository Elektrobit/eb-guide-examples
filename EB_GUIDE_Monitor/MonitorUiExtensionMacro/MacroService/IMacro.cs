////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Elektrobit Automotive GmbH
// Alle Rechte vorbehalten. All Rights Reserved.
// Information contained herein is subject to change without notice.
// Elektrobit retains ownership and all other rights in the software and each
// component thereof.
// Any reproduction of the software or components thereof without the prior
// written permission of Elektrobit is prohibited.
////////////////////////////////////////////////////////////////////////////////

namespace MonitorUiExtensionMacro.MacroService
{
    using System.IO;
    using System.Threading.Tasks;

    /// <summary>
    /// Macro interface
    /// </summary>
    public interface IMacro
    {
        /// <summary>
        /// Serializes the macro to the given stream
        /// </summary>
        /// <param name="stream">The stream to serialize to</param>
        /// <returns>Task when finished has serialized the macro</returns>
        Task Serialize(Stream stream);
    }
}
