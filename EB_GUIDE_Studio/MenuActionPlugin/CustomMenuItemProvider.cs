namespace MenuActionPlugin
{
    using System.Collections.Generic;
    using System.ComponentModel.Composition;
    using System.Threading.Tasks;

    using Elektrobit.Guide.Scheduler;
    using Elektrobit.Guide.Studio.Model.Actions;
    using Elektrobit.Guide.Studio.Model.Actions.Events;
    using Elektrobit.Guide.Studio.Model.Elements.Event;
    using Elektrobit.Guide.Studio.Workbench.ViewModels;
    using Elektrobit.Guide.Ui.ViewModels;
    using Elektrobit.Guide.Utilities;

    using ReactiveUI;

    [Export(typeof(IMenuItemProvider))]
    internal class CustomMenuItemProvider : IMenuItemProvider
    {
        // The unique id for the menu used by the factory and providers
        public const string ID = "custom-actions-menu";

        private readonly IEventService _eventService;

        private readonly ITaskSchedulerProvider _schedulerProvider;

        // We're providing actions for the custom menu.
        public string MenuId => ID;

        [ImportingConstructor]
        public CustomMenuItemProvider(
            IEventService eventService,
            ITaskSchedulerProvider schedulerProvider)
        {
            _eventService = eventService;
            _schedulerProvider = schedulerProvider;
        }

        public IEnumerable<IMenuItemViewModel> CreateMenuItems(object context)
        {
            // Ensure that context is a IWorkbenchViewModel
            Precondition.As<IWorkbenchViewModel>(context, nameof(context));

            yield return new MenuItemViewModel
                               {
                                   Header = "Create dummy event",
                                   Command = ReactiveCommand.CreateFromTask<IWorkbenchViewModel>(CreateDummyEvent),
                                   CommandParameter = context
                               };
        }

        private async Task CreateDummyEvent(IWorkbenchViewModel workbenchViewModel)
        {
            var projectContext = workbenchViewModel.ProjectContext;

            // Model actions execute asynchronously on a special thread so we have to await the result.
            var evt = await _schedulerProvider.ExecuteModelAction(
                          projectContext,
                          session => _eventService.CreateEvent(
                              session,
                              projectContext,
                              projectContext.Project.RootNamespace,
                              "Dummy event"));

            ShowEventToastNotification(workbenchViewModel, evt);
        }

        private static void ShowEventToastNotification(
            IWorkbenchViewModel workbenchViewModel,
            IEvent evt)
        {
            var toastNotification = new ToastNotification
                                        {
                                            Header = "Event created",
                                            Content = $"Event {evt.Name} has been created.",
                                            Priority = ToastNotificationPriority.Success
                                        };

            workbenchViewModel.ToastNotifier.Show(toastNotification);
        }
    }
}
