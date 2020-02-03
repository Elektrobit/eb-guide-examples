////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Elektrobit Automotive GmbH
// Alle Rechte vorbehalten. All Rights Reserved.
// Information contained herein is subject to change without notice.
// Elektrobit retains ownership and all other rights in the software and each
// component thereof.
// Any reproduction of the software or components thereof without the prior
// written permission of Elektrobit is prohibited.
////////////////////////////////////////////////////////////////////////////////

namespace MonitorUiExtensionMacro.UI.ViewModels
{
    using System;
    using System.Collections.Generic;
    using System.ComponentModel.Composition;
    using System.Linq;

    using Elektrobit.Guide.Monitor.Service;
    using Elektrobit.Guide.Monitor.Ui.Dialog;
    using Elektrobit.Guide.Monitor.Ui.Docking;
    using Elektrobit.Guide.Utilities;

    using MonitorUiExtensionMacro.MacroService;

    [Export(typeof(IDockableComponentFactory))]
    public class MacroViewModelFactory : IDockableComponentFactory
    {
        private readonly ILoggerService _loggerService;

        private readonly IMonitorDialog _monitorDialog;

        private readonly IFileService _fileService;

        private readonly IMacroService _macroService;

        [ImportingConstructor]
        public MacroViewModelFactory(
            ILoggerService loggerService,
            IMonitorDialog monitorDialog,
            IFileService fileService,
            IMacroService macroService)
        {
            _loggerService = loggerService;
            _monitorDialog = monitorDialog;
            _fileService = fileService;
            _macroService = macroService;

            ContentIdBase = "monitor_macro_view";
            Title = "Macro Recorder";
        }

        public IDockableComponentViewModel CreateComponent()
        {
            return new MacroViewModel(_loggerService, _monitorDialog, _fileService, _macroService);
        }

        public string ContentIdBase { get; }

        public string Title { get; }
    }
}
