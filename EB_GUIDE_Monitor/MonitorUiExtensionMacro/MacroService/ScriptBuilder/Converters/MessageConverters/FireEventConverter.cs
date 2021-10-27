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
    using System.Linq;

    using Elektrobit.Guide.Monitor.Messages.Events;
    using Elektrobit.Guide.Monitor.Scripting.MonitorContext;
    using Elektrobit.Guide.Monitor.Service;
    using Elektrobit.Guide.Monitor.Service.Namespace;
    using Elektrobit.Guide.Monitor.Utilities;

    using MonitorUiExtensionMacro.MacroService.ScriptBuilder.Template;

    [ExportMessageScriptConverter(typeof(IMessageToScriptConverter), typeof(FireEvent))]
    internal class FireEventConverter : MessageConverterBase<FireEvent>
    {
        [ImportingConstructor]
        public FireEventConverter(IModelService modelService, INamespaceService namespaceService, IModelValueConverter valueConverter)
            : base(modelService, namespaceService, valueConverter)
        {
        }

        protected override ScriptAction Convert(string monitorContextId, FireEvent eventMessage)
        {
            var eventName = GetEventFullName(eventMessage.EventId, eventMessage.GroupId);

            return ScriptAction.CreateAwaitedContextMethodCall(monitorContextId,
                nameof(IMonitorContext.FireEvent),
                CreateParameterList(eventName).Concat(CreateParameterList(eventMessage.Parameters)));
        }
    }
}
