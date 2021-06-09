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
    using System.ComponentModel.Composition;

    public interface IMacroStepConverterMetadata
    {
        Type MacroStepType { get; }
    }

    [MetadataAttribute]
    [AttributeUsage(AttributeTargets.Class)]
    public class ExportMacroStepConverterAttribute : ExportAttribute, IMacroStepConverterMetadata
    {
        public Type MacroStepType { get; }

        public ExportMacroStepConverterAttribute(Type contract, Type macroStepType)
            : base(contract)
        {
            MacroStepType = macroStepType;
        }
    }
}