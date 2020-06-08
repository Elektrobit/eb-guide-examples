////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Elektrobit Automotive GmbH
// Alle Rechte vorbehalten. All Rights Reserved.
// Information contained herein is subject to change without notice.
// Elektrobit retains ownership and all other rights in the software and each
// component thereof.
// Any reproduction of the software or components thereof without the prior
// written permission of Elektrobit is prohibited.
////////////////////////////////////////////////////////////////////////////////

namespace MonitorUiExtensionMacro.MacroService.Macros
{
    using System.Collections.Generic;
    using System.IO;
    using System.Text;
    using System.Threading.Tasks;

    internal class MessageMacro : IMacro
    {
        private readonly IMacroToScriptBuilder _builder;

        public IEnumerable<IMacroStep> Steps { get; }

        public MessageMacro(IEnumerable<IMacroStep> steps, IMacroToScriptBuilder builder)
        {
            _builder = builder;
            Steps = steps;
        }

        public async Task Serialize(Stream stream)
        {
            var built = await _builder.Build(this);
            var bytes = Encoding.UTF8.GetBytes(built);
            stream.Write(bytes, 0, bytes.Length);
        }
    }
}
