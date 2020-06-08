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
    using System.ComponentModel.Composition;

    [Export(typeof(IMacroRecordSessionFactory))]
    public class MacroRecordSessionFactory : IMacroRecordSessionFactory
    {
        private readonly IMacroFactory _macroFactory;

        [ImportingConstructor]
        public MacroRecordSessionFactory(IMacroFactory macroFactory)
        {
            _macroFactory = macroFactory;
        }

        public IMacroRecordSession Create(IMacroRecorder recorder)
        {
            return new MacroRecordSession(_macroFactory, recorder);
        }
    }
}