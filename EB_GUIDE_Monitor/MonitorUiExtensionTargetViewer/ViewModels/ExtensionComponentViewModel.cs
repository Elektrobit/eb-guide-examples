////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Elektrobit Automotive GmbH
// Alle Rechte vorbehalten. All Rights Reserved.
// Information contained herein is subject to change without notice.
// Elektrobit retains ownership and all other rights in the software and each
// component thereof.
// Any reproduction of the software or components thereof without the prior
// written permission of Elektrobit is prohibited.
////////////////////////////////////////////////////////////////////////////////

namespace MonitorUiExtension.ViewModels
{
    using System;
    using System.Configuration;
    using System.Reflection;
    using System.Windows.Media.Imaging;

    using Elektrobit.Guide.Monitor.Service;
    using Elektrobit.Guide.Monitor.Service.Configuration;
    using Elektrobit.Guide.Monitor.Service.ScreenshotService;
    using Elektrobit.Guide.Monitor.Ui.Configuration;
    using Elektrobit.Guide.Monitor.Ui.Docking;
    using Elektrobit.Guide.Monitor.Utilities;
    using Elektrobit.Guide.Options;
    using Elektrobit.Guide.Ui.Docking.ViewModels;
    using Elektrobit.Guide.Ui.Mvvm;

    using Prism.Commands;

    public class ExtensionComponentViewModel : ViewModel, IDockableComponentViewModel
    {
        private readonly IConnectionService _connectionService;

        private readonly IScreenshotService _screenshotService;

        private readonly ILoggerService _loggerService;

        private readonly IScreenshotSavingUtility _screenshotSavingUtility;

        private readonly IConfigureOptions<ExtensionComponentOptions> _options;

        public DelegateCommand Refresh { get; private set; }

        public DelegateCommand Save { get; private set; }

        private BitmapSource _screenshotSource;

        private string _selectedScene;

        /// <summary>
        /// Property to bring screenshot into view
        /// </summary>
        public BitmapSource ScreenshotSource
        {
            get => _screenshotSource;

            set
            {
                // set the value 
                SetProperty(ref _screenshotSource, value);

                // enable save button if it is still disabled
                Save.RaiseCanExecuteChanged();
            }
        }

        public string SelectedScene
        {
            get => _selectedScene;
            set => SetProperty(ref _selectedScene, value);
        }

        public ExtensionComponentViewModel(
            IConnectionService connectionService,
            IScreenshotService screenshotService,
            IScreenshotSavingUtility screenshotSavingUtility,
            ILoggerService loggerService,
            IConfigurationService configurationService,
            WorkbenchPartId componentId)
        {
            _connectionService = connectionService;
            _screenshotService = screenshotService;
            _loggerService = loggerService;
            _screenshotSavingUtility = screenshotSavingUtility;

            InitializeCommands();

            _options = new OptionsBuilder<ExtensionComponentOptions>(configurationService.Model)
                .ForComponent(componentId)
                .BeforeDeserialize(InitializeOptions)
                .AfterDeserialize(ApplyOptions)
                .BeforeSerialize(UpdateOptions)
                .Build();
        }

        private void InitializeCommands()
        {
            Save = new DelegateCommand(OnSave, CanSave);

            Refresh = new DelegateCommand(OnRefresh, CanRefresh);

            // react on connection status has changed
            _connectionService.Connected += (sender, args) => Refresh.RaiseCanExecuteChanged();
            _connectionService.Disconnected += (sender, args) => Refresh.RaiseCanExecuteChanged();
        }

        private bool CanSave()
        {
            // can save screenshots only if received before
            return ScreenshotSource != null;
        }

        private void OnSave()
        {
            var path = GetPathForScreenShot();

            // use service to store screenshot to disk
            _screenshotSavingUtility.SaveScreenshotFromBitmapSource(ScreenshotSource, path);

            // log message in gui
            _loggerService.LogMessage(this, $"Screenshot saved to {path}", Severity.Info);
        }

        private static string GetPathForScreenShot()
        {
            var currentAssemblyLocation = Assembly.GetExecutingAssembly().Location;
            var assemblyConfig = ConfigurationManager.OpenExeConfiguration(currentAssemblyLocation);
            var path = assemblyConfig.AppSettings?.Settings["ScreenShotPath"]?.Value;
            
            if (string.IsNullOrEmpty(path))
            {
                return @"c:\Temp\monitor_screenshot.png";
            }

            return path;
        }

        private bool CanRefresh()
        {
            // refresh only possible if connection is established
            return _connectionService.IsConnected;
        }

        private async void OnRefresh()
        {
            try
            {
                // read screenshot with help of service
                var screenshot = await _screenshotService.GetScreenshotAsync(uint.Parse(SelectedScene));

                // bring screenshot to view
                ScreenshotSource = screenshot.BitmapSource;
            }
            catch (Exception ex)
            {
                // bring errors to view in logger component
                // e.g. when remote frame buffer is not enabled for scene
                _loggerService.LogMessage(this, $"Error in monitor extension: {ex.Message}", Severity.Error);
            }
        }

        public void Dispose()
        {
            _options.Dispose();
        }

        private void InitializeOptions(ExtensionComponentOptions options)
        {
            options.SelectedScene = "0";
        }

        private void ApplyOptions(ExtensionComponentOptions options)
        {
            SelectedScene = options.SelectedScene;
        }

        private void UpdateOptions(ExtensionComponentOptions options)
        {
            options.SelectedScene = SelectedScene;
        }
    }
}
