////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Elektrobit Automotive GmbH
// Alle Rechte vorbehalten. All Rights Reserved.
// Information contained herein is subject to change without notice.
// Elektrobit retains ownership and all other rights in the software and each
// component thereof.
// Any reproduction of the software or components thereof without the prior
// written permission of Elektrobit is prohibited.
////////////////////////////////////////////////////////////////////////////////

namespace MonitorUiExtensionMacro.MacroService.ScriptBuilder.Template
{
    using System.Collections.Generic;
    using System.Linq;

    public class ScriptAction
    {
        public string Action { get; }

        public ScriptAction(string action)
        {
            Action = action;
        }

        public static ScriptAction CreateAwaitedContextMethodCall(string instance, string methodName, IEnumerable<string> parameters)
        {
            var scriptString = $"await {instance}.{methodName}({parameters.Aggregate((i,j) => i + ", " + j)});";
            return new ScriptAction(scriptString);
        }
    }
}
