////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Elektrobit Automotive GmbH
// Alle Rechte vorbehalten. All Rights Reserved.
// Information contained herein is subject to change without notice.
// Elektrobit retains ownership and all other rights in the software and each
// component thereof.
// Any reproduction of the software or components thereof without the prior
// written permission of Elektrobit is prohibited.
////////////////////////////////////////////////////////////////////////////////

namespace MonitorUiExtensionMacro.MacroService.ScriptBuilder.Converters
{
    using MonitorUiExtensionMacro.MacroService.ScriptBuilder.Template;

    [ExportMacroStepConverter(typeof(IMacroStepToScriptConverter), typeof(MacroDelayStep))]
    public class DelayStepToScriptConverter : IMacroStepToScriptConverter
    {
        public ScriptAction Convert(string monitorContextName, IMacroStep step)
        {
            var delay = step.Get<Delay>().Milliseconds;
            return new ScriptAction($"await Task.Delay({delay});");
        }
    }
}