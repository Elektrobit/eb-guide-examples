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
    using System.Collections.Generic;

    /// <summary>
    /// Macro factory interface
    /// </summary>
    public interface IMacroFactory
    {
        /// <summary>
        /// Creates <see cref="IMacro"/> from given <see cref="IMacroStep"/>
        /// </summary>
        /// <param name="steps">The collection <see cref="IMacroStep"/> to convert to a <see cref="IMacro"/></param>
        /// <returns>The created macro</returns>
        IMacro Create(IEnumerable<IMacroStep> steps);
    }
}