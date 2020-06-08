////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Elektrobit Automotive GmbH
// Alle Rechte vorbehalten. All Rights Reserved.
// Information contained herein is subject to change without notice.
// Elektrobit retains ownership and all other rights in the software and each
// component thereof.
// Any reproduction of the software or components thereof without the prior
// written permission of Elektrobit is prohibited.
////////////////////////////////////////////////////////////////////////////////

namespace MonitorUiExtensionMacro.MacroService.ScriptBuilder.Converters.MessageConverters
{
    using System.ComponentModel.Composition;

    using Elektrobit.Guide.Monitor.Messages.Datapool;
    using Elektrobit.Guide.Monitor.Scripting.MonitorContext;
    using Elektrobit.Guide.Monitor.Service;
    using Elektrobit.Guide.Monitor.Utilities;

    using MonitorUiExtensionMacro.MacroService.ScriptBuilder.Template;

    [ExportMessageScriptConverter(typeof(IMessageToScriptConverter), typeof(WriteScalarValue))]
    internal class WriteScalarConverter : MessageConverterBase<WriteScalarValue>
    {
        [ImportingConstructor]
        public WriteScalarConverter(IModelService modelService, IModelValueConverter valueConverter)
            : base(modelService, valueConverter)
        {
        }

        protected override ScriptAction Convert(string monitorContextId, WriteScalarValue valueMessage)
        {
            var itemName = GetDatapoolItemName(valueMessage.DatapoolItemId);
            return ScriptAction.CreateAwaitedContextMethodCall(monitorContextId,
                nameof(IMonitorContext.WriteDatapool),
                CreateParameterList(itemName, valueMessage.Value));
        }
    }
}
