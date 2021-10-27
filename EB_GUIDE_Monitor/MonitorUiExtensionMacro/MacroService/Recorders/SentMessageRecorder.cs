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
    using System.Collections.Generic;
    using System.ComponentModel.Composition;

    using Elektrobit.Guide.Monitor.Connection;
    using Elektrobit.Guide.Monitor.Messages.Datapool;
    using Elektrobit.Guide.Monitor.Service;
    using Elektrobit.Guide.Utilities;

    [Export(typeof(IMacroRecorder))]
    public class SentMessageRecorder : MacroRecorderBase, IMacroRecorder
    {
        private readonly IConnectionService _connectionService;

        public string Name => "Message Recorder!";

        [ImportingConstructor]
        public SentMessageRecorder(IConnectionService connectionService, IDateTimeService dateTimeService)
            : base(dateTimeService, true)
        {
            _connectionService = connectionService;
        }

        private void ConnectionServiceOnMessageSent(object sender, MessageSentEventArgs e)
        {
            // ignored messages TODO should these be filtered with some input from ScriptBuilder, what message types it supports?
            if(e.Message is Commit
               || e.Message is ListLengthRequest
               || e.Message is ListItemRequest)
            {
                return;
            }

            AddStep(new MessageStep(e.Message));
        }

        public void Start()
        {
            ClearSteps();
            _connectionService.MessageSent += ConnectionServiceOnMessageSent; 
        }

        public IEnumerable<IMacroStep> Stop()
        {
            _connectionService.MessageSent -= ConnectionServiceOnMessageSent;
            return Steps;
        }
    }
}