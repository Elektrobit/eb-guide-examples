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
    using System.Threading.Tasks;

    /// <summary>
    /// Interface for converting <see cref="IMacro"/> to a Monitor script string
    /// </summary>
    public interface IMacroToScriptBuilder
    {
        /// <summary>
        /// Builds script from a <see cref="IMacro"/>
        /// </summary>
        /// <param name="macro">The <see cref="IMacro"/> to convert</param>
        /// <returns>Task which on success will result in Monitor script</returns>
        Task<string> Build(IMacro macro);
    }
}
