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

    [ExportMessageScriptConverter(typeof(IMessageToScriptConverter), typeof(RemoveListItem))]
    internal class RemoveListItemConverter : MessageConverterBase<RemoveListItem>
    {
        [ImportingConstructor]
        public RemoveListItemConverter(IModelService modelService, IModelValueConverter valueConverter)
            : base(modelService, valueConverter)
        {
        }

        protected override ScriptAction Convert(string monitorContextId, RemoveListItem removeMessage)
        {
            var itemName = GetDatapoolItemName(removeMessage.ItemId);

            return ScriptAction.CreateAwaitedContextMethodCall(monitorContextId,
                nameof(IMonitorContext.RemoveDatapoolListItem),
                CreateParameterList(itemName, removeMessage.Index));
        }
    }
}
