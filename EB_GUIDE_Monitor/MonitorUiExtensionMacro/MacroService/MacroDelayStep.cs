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
    public class MacroDelayStep : IMacroStep
    {
        private readonly Delay _delay;

        public MacroDelayStep(Delay delay)
        {
            _delay = delay;
        }

        public T Get<T>() where T : class
        {
            return _delay as T;
        }
    }
}