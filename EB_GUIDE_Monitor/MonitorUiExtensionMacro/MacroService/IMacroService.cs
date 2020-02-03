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
    /// MacroService for discovering <see cref="IMacroRecorder"/>s and utilizing them
    /// </summary>
    public interface IMacroService
    {
        /// <summary>
        /// Creates instances from found <see cref="IMacroRecorder"/> implementations
        /// </summary>
        IEnumerable<IMacroRecorder> CreateRecorders();

        /// <summary>
        /// Creates new <see cref="IMacroRecordSession"/> with given <see cref="IMacroRecorder"/>
        /// </summary>
        /// <param name="recorder">The <see cref="IMacroRecorder"/> to create session for</param>
        /// <returns><see cref="IMacroRecordSession"/> that can be used to record macros</returns>
        IMacroRecordSession CreateSession(IMacroRecorder recorder);
    }
}