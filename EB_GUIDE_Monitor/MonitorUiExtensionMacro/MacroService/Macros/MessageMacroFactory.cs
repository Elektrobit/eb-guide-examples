////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Elektrobit Automotive GmbH
// Alle Rechte vorbehalten. All Rights Reserved.
// Information contained herein is subject to change without notice.
// Elektrobit retains ownership and all other rights in the software and each
// component thereof.
// Any reproduction of the software or components thereof without the prior
// written permission of Elektrobit is prohibited.
////////////////////////////////////////////////////////////////////////////////

namespace MonitorUiExtensionMacro.MacroService.Macros
{
    using System.Collections.Generic;
    using System.ComponentModel.Composition;

    [Export(typeof(IMacroFactory))]
    public class MessageMacroFactory : IMacroFactory
    {
        private readonly IMacroToScriptBuilder _scriptBuilder;

        [ImportingConstructor]
        public MessageMacroFactory(IMacroToScriptBuilder scriptBuilder)
        {
            _scriptBuilder = scriptBuilder;
        }

        public IMacro Create(IEnumerable<IMacroStep> steps)
        {
            return new MessageMacro(steps, _scriptBuilder);
        }
    }
}