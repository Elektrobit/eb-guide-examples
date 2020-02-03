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
    using System.ComponentModel.Composition;

    using Elektrobit.Guide.Monitor.Service;
    using Elektrobit.Guide.Monitor.Service.ScreenshotService;
    using Elektrobit.Guide.Monitor.Ui.Docking;
    using Elektrobit.Guide.Monitor.Utilities;

    [Export(typeof(IDockableComponentFactory))]
    public class ExtensionViewModelFactory : IDockableComponentFactory
    {
        private readonly IScreenshotService _screenshotService;

        private readonly IConnectionService _connectionService;
        
        private readonly ILoggerService _loggerService;

        private readonly IScreenshotSavingUtility _screenshotSavingUtility;

        public string ContentIdBase { get; }

        public string Title { get; }

        [ImportingConstructor]
        public ExtensionViewModelFactory(IConnectionService connectionService, IScreenshotService screenshotService, IScreenshotSavingUtility screenshotSavingUtility, ILoggerService loggerService)
        {
            Title = "Monitor Target Viewer Extension";
            ContentIdBase = "monitor_extension_target_viewer_id";

            _screenshotService = screenshotService;
            _connectionService = connectionService;
            _loggerService = loggerService;
            _screenshotSavingUtility = screenshotSavingUtility;
        }

        public IDockableComponentViewModel CreateComponent()
        {
            return new ExtensionComponentViewModel(_connectionService, _screenshotService, _screenshotSavingUtility, _loggerService);
        }
    }
}
