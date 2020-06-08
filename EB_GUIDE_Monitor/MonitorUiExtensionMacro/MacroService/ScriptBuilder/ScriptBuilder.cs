////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Elektrobit Automotive GmbH
// Alle Rechte vorbehalten. All Rights Reserved.
// Information contained herein is subject to change without notice.
// Elektrobit retains ownership and all other rights in the software and each
// component thereof.
// Any reproduction of the software or components thereof without the prior
// written permission of Elektrobit is prohibited.
////////////////////////////////////////////////////////////////////////////////

namespace MonitorUiExtensionMacro.MacroService.ScriptBuilder
{
    using System;
    using System.Collections.Generic;
    using System.ComponentModel.Composition;
    using System.Linq;
    using System.Threading.Tasks;

    using MonitorUiExtensionMacro.MacroService.Macros;
    using MonitorUiExtensionMacro.MacroService.ScriptBuilder.Converters;
    using MonitorUiExtensionMacro.MacroService.ScriptBuilder.Template;

    [Export(typeof(IMacroToScriptBuilder))]
    public class ScriptBuilder : IMacroToScriptBuilder
    {
        private const string MONITOR_CONTEXT_NAME = "monitor";

        private readonly List<Lazy<IMacroStepToScriptConverter, IMacroStepConverterMetadata>> _converters;

        [ImportingConstructor]
        public ScriptBuilder([ImportMany] IEnumerable<Lazy<IMacroStepToScriptConverter, IMacroStepConverterMetadata>> converters)
        {
            _converters = new List<Lazy<IMacroStepToScriptConverter, IMacroStepConverterMetadata>>(converters);
        }

        public Task<string> Build(IMacro macro)
        {
            var messageMacro = (MessageMacro)macro;
            return Task.Factory.StartNew(() => GenerateScript(messageMacro.Steps));
        }

        private string GenerateScript(IEnumerable<IMacroStep> steps)
        {
            var template = new MonitorScriptTemplate
            {
                ClassName = "TestClass",
                MethodName = "ScriptMethod",
                Actions = steps.Select(ConvertStep),
            };

            return template.TransformText();
        }

        private ScriptAction ConvertStep(IMacroStep step)
        {
            var converter = _converters.FirstOrDefault(c => c.Metadata.MacroStepType == step.GetType())?.Value;
            if (converter != null)
            {
                return converter.Convert(MONITOR_CONTEXT_NAME, step);
            }
            else
            {
                return new ScriptAction($"// no converter for '{step.GetType().Name}'!");
            }
        }
    }
}
