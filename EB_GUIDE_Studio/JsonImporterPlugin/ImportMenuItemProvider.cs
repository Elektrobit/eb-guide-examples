////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Elektrobit Automotive GmbH
// Alle Rechte vorbehalten. All Rights Reserved.
//
// Information contained herein is subject to change without notice.
// Elektrobit retains ownership and all other rights in the software and each
// component thereof.
// Any reproduction of the software or components thereof without the prior
// written permission of Elektrobit is prohibited.
////////////////////////////////////////////////////////////////////////////////

namespace JsonImporterPlugin
{
    using System.Collections.Generic;
    using System.ComponentModel.Composition;
    using System.IO;
    using System.Threading.Tasks;

    using Elektrobit.Guide.Options;
    using Elektrobit.Guide.Scheduler;
    using Elektrobit.Guide.Studio.Configuration;
    using Elektrobit.Guide.Studio.Workbench.ViewModels;
    using Elektrobit.Guide.Ui.ViewModels;
    using Elektrobit.Guide.Utilities;

    using Fclp.Internals.Extensions;

    using JsonImporterPlugin.Deserializer;
    using JsonImporterPlugin.Model;
    using JsonImporterPlugin.Service;

    using ReactiveUI;

    using ExecutionMode = Elektrobit.Guide.Scheduler.ExecutionMode;

    [Export(typeof(IMenuItemProvider))]
    internal class ImportMenuItemProvider : IMenuItemProvider
    {
        public const string ID = "import-actions-menu";

        private readonly IJsonModelService _jsonModelService;

        private readonly IJsonFileReader _jsonFileReader;

        private readonly ITaskSchedulerProvider _schedulerProvider;

        private readonly IConfigureOptions<ImportOptions> _options;

        public string MenuId => ID;

        [ImportingConstructor]
        public ImportMenuItemProvider(
            IJsonModelService jsonModelService,
            IJsonFileReader jsonFileReader,
            IConfigurationService configurationService,
            ITaskSchedulerProvider schedulerProvider)
        {
            _jsonModelService = jsonModelService;
            _jsonFileReader = jsonFileReader;
            _schedulerProvider = schedulerProvider;

            // Options are automatically loaded on startup/when loading a model,
            // and saved when the program exits.
            // Use the OptionsBuilder extension methods to define a unique key for your plug-in,
            // or to perform additional actions when the options are loaded (deserialized)
            // or stored (serialized) on disk.
            // In this example, the options are read and updated directly when a file is selected.
            _options = new OptionsBuilder<ImportOptions>(configurationService.Model)
                .ForSection("JsonImporterPlugin", "ImportMenu").AfterDeserialize(ApplyOptions).BeforeSerialize(UpdateOptions)
                .Build();
        }

        private void ApplyOptions(ImportOptions options)
        {
            // The options have been loaded from persistence.
            // Transfer them to the provider's internal members, if necessary.
        }

        private void UpdateOptions(ImportOptions options)
        {
            // The options are about to be saved.
            // Update the passed options with the current provider's state, if necessary.
        }

        public IEnumerable<IMenuItemViewModel> CreateMenuItems(object context)
        {
            Precondition.As<IWorkbenchViewModel>(context, nameof(context));

            yield return new MenuItemViewModel
            {
                Header = "Import model from JSON...",
                Command = ReactiveCommand.CreateFromTask<IWorkbenchViewModel>(ImportJsonModel),
                CommandParameter = context
            };
        }

        private async Task ImportJsonModel(IWorkbenchViewModel workbenchViewModel)
        {
            var projectContext = workbenchViewModel.ProjectContext;
            var initialDir = _options.Value.ImportLocation;
            if (string.IsNullOrEmpty(initialDir))
            {
                initialDir = projectContext.Location;
            }

            var jsonFile = _jsonFileReader.SelectJsonFile(initialDir);
            if (jsonFile.IsNullOrEmpty())
            {
                return;
            }

            _options.Value.ImportLocation = Path.GetDirectoryName(jsonFile);

            var result = await _schedulerProvider.PoolScheduler.InvokeAsync(
                             () => _jsonModelService.ImportModelData(
                                 projectContext,
                                 jsonFile),
                             executionMode: ExecutionMode.Blocking);

            ShowImportToastNotification(workbenchViewModel, result);
        }

        private static void ShowImportToastNotification(IWorkbenchViewModel workbenchViewModel, ImportModelDataResult result)
        {
            var priority = result.IsSuccess ? ToastNotificationPriority.Success : ToastNotificationPriority.Warning;

            var toastNotification = new ToastNotification
            {
                Header = result.Title,
                Content = result.Message,
                Priority = priority
            };

            workbenchViewModel.ToastNotifier.Show(toastNotification);
        }
    }
}
