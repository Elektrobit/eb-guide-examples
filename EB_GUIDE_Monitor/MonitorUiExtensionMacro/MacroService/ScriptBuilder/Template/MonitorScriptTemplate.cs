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

    public partial class MonitorScriptTemplate
    {
        public string ClassName { get; set; }
        public string MethodName { get; set; }
        public IEnumerable<ScriptAction> Actions { get; set; }

        protected void WriteAwaitedMethodCall(ScriptAction action)
        {
            if(string.IsNullOrEmpty(action.Action))
            {
                return;
            }

            WriteLine(action.Action);
        }
    }
}