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
    /// Macro recorder interface
    /// </summary>
    public interface IMacroRecorder
    {
        /// <summary>
        /// Name of the recorder
        /// </summary>
        string Name { get; }

        /// <summary>
        /// Start recording
        /// </summary>
        void Start();

        /// <summary>
        /// Stop recording
        /// </summary>
        /// <returns>The stored <see cref="IMacroStep"/>s</returns>
        IEnumerable<IMacroStep> Stop();
    }
}