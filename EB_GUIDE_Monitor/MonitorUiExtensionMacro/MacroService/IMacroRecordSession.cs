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
    /// <summary>
    /// Macro record session
    /// </summary>
    public interface IMacroRecordSession
    {
        /// <summary>
        /// Start recording session
        /// </summary>
        void Start();

        /// <summary>
        /// Stop session
        /// </summary>
        /// <returns>The recorded macro</returns>
        IMacro Stop();
    }
}
