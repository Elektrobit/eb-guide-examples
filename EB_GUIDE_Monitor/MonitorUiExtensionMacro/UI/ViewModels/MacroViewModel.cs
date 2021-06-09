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
    using System.Collections.Generic;
    using System.Collections.ObjectModel;
    using System.Linq;

    using Elektrobit.Guide.Monitor.Service;
    using Elektrobit.Guide.Monitor.Ui.Dialog;
    using Elektrobit.Guide.Monitor.Ui.Docking;
    using Elektrobit.Guide.Ui.Mvvm;
    using Elektrobit.Guide.Utilities;

    using MonitorUiExtensionMacro.MacroService;

    using Prism.Mvvm;

    public class MacroViewModel : BindableBase, IDockableComponentViewModel
    {
        private readonly ILoggerService _loggerService;

        private readonly IMonitorDialog _monitorDialog;

        private readonly IFileService _fileService;

        private readonly IMacroService _macroService;

        private IMacroRecordSession _runningRecordSession;

        public bool IsRunning { get; }

        public DelegateCommandAsync Record { get; }

        public DelegateCommandAsync Stop { get; }

        public DelegateCommandAsync Build { get; }

        public IEnumerable<IMacroRecorder> Recorders { get; }

        public IMacroRecorder SelectedRecorder => Recorders.First();

        public ObservableCollection<IMacro> SavedMacros;

        public IViewModel Parent { get; set; }

        public MacroViewModel(
            ILoggerService loggerService,
            IMonitorDialog monitorDialog,
            IFileService fileService,
            IMacroService macroService)
        {
            IsRunning = false;
            SavedMacros = new ObservableCollection<IMacro>();

            _loggerService = loggerService;
            _monitorDialog = monitorDialog;
            _fileService = fileService;
            _macroService = macroService;

            Recorders = _macroService.CreateRecorders();

            Record = new DelegateCommandAsync(() =>
                    {
                        _runningRecordSession = _macroService.CreateSession(SelectedRecorder);
                        _runningRecordSession?.Start();
                        Stop.RaiseCanExecuteChanged();
                        Record.RaiseCanExecuteChanged();
                    },
                () => SelectedRecorder != null && _runningRecordSession == null);

            Stop = new DelegateCommandAsync(() =>
                    {
                        var macro = _runningRecordSession.Stop();
                        _runningRecordSession = null;
                        SavedMacros.Add(macro);
                        Stop.RaiseCanExecuteChanged();
                        Record.RaiseCanExecuteChanged();
                        Build.RaiseCanExecuteChanged();
                    },
                () => _runningRecordSession != null);

            Build = new DelegateCommandAsync(async () =>
                {
                    var macro = SavedMacros.First();
                    SavedMacros.RemoveAt(0);

                    Build.RaiseCanExecuteChanged();

                    var saveResult = _monitorDialog.SaveFileDialog("Save script", "test_script.cs", "Monitor Script (*.cs)|*.cs;|All files (*.*)|*.*", null);
                    if (saveResult.Success)
                    {
                        using (var stream = _fileService.CreateNewFileStream(saveResult.FileName))
                        {
                            await macro.Serialize(stream);
                            _loggerService.LogMessage(this, $"Script built & saved '{saveResult.FileName}'!", Severity.Info);
                        }
                    }
                },
               () => SavedMacros.Any());
        }

        public void Dispose()
        {
        }
    }
}
