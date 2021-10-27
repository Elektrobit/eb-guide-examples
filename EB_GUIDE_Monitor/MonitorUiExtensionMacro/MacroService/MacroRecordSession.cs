////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Elektrobit Automotive GmbH
// Alle Rechte vorbehalten. All Rights Reserved.
// Information contained herein is subject to change without notice.
// Elektrobit retains ownership and all other rights in the software and each
// component thereof.
// Any reproduction of the software or components thereof without the prior
// written permission of Elektrobit is prohibited.
////////////////////////////////////////////////////////////////////////////////

namespace MonitorUiExtensionMacro.MacroService
{
    using System.IO;

    public class MacroRecordSession : IMacroRecordSession
    {
        private readonly IMacroFactory _macroFactory;

        private readonly IMacroRecorder _recorder;

        public MacroRecordSession(IMacroFactory macroFactory, IMacroRecorder recorder)
        {
            _macroFactory = macroFactory;
            _recorder = recorder;
        }

        public void Start()
        {
            _recorder.Start();
        }

        public IMacro Stop()
        {
            return _macroFactory.Create(_recorder.Stop());
        }
    }
}
