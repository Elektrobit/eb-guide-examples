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
    using Elektrobit.Guide.Monitor.Connection.Protocols;

    using MonitorUiExtensionMacro.MacroService.ScriptBuilder.Template;

    public interface IMessageToScriptConverter
    {
        ScriptAction Convert(string monitorContextId, IMessage message);
    }
}