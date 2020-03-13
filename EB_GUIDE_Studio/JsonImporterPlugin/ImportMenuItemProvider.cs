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
    using System.Threading.Tasks;

    using Elektrobit.Guide.Scheduler;
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

        public string MenuId => ID;

        [ImportingConstructor]
        public ImportMenuItemProvider(
            IJsonModelService jsonModelService,
            IJsonFileReader jsonFileReader,
            ITaskSchedulerProvider schedulerProvider)
        {
            _jsonModelService = jsonModelService;
            _jsonFileReader = jsonFileReader;
            _schedulerProvider = schedulerProvider;
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
            var jsonFile = _jsonFileReader.SelectJsonFile(projectContext);

            if (!jsonFile.IsNullOrEmpty())
            {
                var result = await _schedulerProvider.PoolScheduler.InvokeAsync(
                                 () => _jsonModelService.ImportModelData(
                                     projectContext,
                                     jsonFile),
                                 executionMode: ExecutionMode.Blocking);

                ShowImportToastNotification(workbenchViewModel, result);
            }
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
