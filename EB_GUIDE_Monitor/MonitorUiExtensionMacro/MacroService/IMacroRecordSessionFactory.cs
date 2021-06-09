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
    /// Factory for creating <see cref="IMacroRecordSession"/>s
    /// </summary>
    public interface IMacroRecordSessionFactory
    {
        /// <summary>
        /// Creates <see cref="IMacroRecordSession"/>
        /// </summary>
        /// <param name="recorder">The <see cref="IMacroRecorder"/> to use in the session</param>
        /// <returns>The session</returns>
        IMacroRecordSession Create(IMacroRecorder recorder);
    }
}