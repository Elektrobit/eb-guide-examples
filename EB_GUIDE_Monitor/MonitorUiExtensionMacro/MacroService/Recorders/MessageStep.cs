////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Elektrobit Automotive GmbH
// Alle Rechte vorbehalten. All Rights Reserved.
// Information contained herein is subject to change without notice.
// Elektrobit retains ownership and all other rights in the software and each
// component thereof.
// Any reproduction of the software or components thereof without the prior
// written permission of Elektrobit is prohibited.
////////////////////////////////////////////////////////////////////////////////

namespace MonitorUiExtensionMacro.MacroService.Recorders
{
    using Elektrobit.Guide.Monitor.Connection.Protocols;

    public class MessageStep : IMacroStep
    {
        private readonly IMessage _message;

        public MessageStep(IMessage message)
        {
           _message = message;
        }

        public T Get<T>() where T : class
        {
            return _message as T;
        }
    }
}
