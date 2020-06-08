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
    using System;
    using System.Collections.Generic;
    using System.ComponentModel.Composition;
    using System.Linq;

    using Elektrobit.Guide.Monitor.Connection.Protocols;

    using MonitorUiExtensionMacro.MacroService.Recorders;
    using MonitorUiExtensionMacro.MacroService.ScriptBuilder.Template;

    [ExportMacroStepConverter(typeof(IMacroStepToScriptConverter), typeof(MessageStep))]
    public class MessageStepToScriptConverter : IMacroStepToScriptConverter
    {
        private readonly List<Lazy<IMessageToScriptConverter, IMessageConverterMetadata>> _converters;

        [ImportingConstructor]
        public MessageStepToScriptConverter([ImportMany] IEnumerable<Lazy<IMessageToScriptConverter, IMessageConverterMetadata>> converters)
        {
            _converters = new List<Lazy<IMessageToScriptConverter, IMessageConverterMetadata>>(converters);
        }

        public ScriptAction Convert(string monitorContextName, IMacroStep step)
        {
            var messageStep = step as MessageStep;
            var message = messageStep.Get<IMessage>();
            var messageType = message.GetType();
            var converter = _converters.FirstOrDefault(c => c.Metadata.MessageType == messageType);

            if (converter != null)
            {
                return converter.Value.Convert(monitorContextName, messageStep.Get<IMessage>());
            }
            else
            {
                return new ScriptAction($"// cannot convert message: '{messageType.Name}");
            }
        }
    }
}
