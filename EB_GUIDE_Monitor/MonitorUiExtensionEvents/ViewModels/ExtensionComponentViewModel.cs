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
    using System.Collections.Generic;

    using Elektrobit.Guide.Monitor.Model.Event;
    using Elektrobit.Guide.Monitor.Model.Value;
    using Elektrobit.Guide.Monitor.Service;
    using Elektrobit.Guide.Monitor.Ui.Docking;
    using Elektrobit.Guide.Ui.Mvvm;

    using Prism.Commands;
    using Prism.Mvvm;

    public class ExtensionComponentViewModel : ViewModel, IDockableComponentViewModel
    {
        private readonly IModelService _modelService;

        private readonly IEventService _eventService;

        private readonly IConnectionService _connectionService;

        private Event _selectedEvent;

        /// <summary>
        /// Binding property for view with list of all events
        /// </summary>
        public IEnumerable<Event> Events { get; set; }

        /// <summary>
        /// Binding property for view for the currently selected event
        /// </summary>
        public Event SelectedEvent
        {
            get => _selectedEvent;

            set
            {
                _selectedEvent = value;
                FireEvent.RaiseCanExecuteChanged();
            }
        }

        /// <summary>
        /// Binding command for reaction if fire event is pressed in GUI
        /// </summary>
        public DelegateCommand FireEvent { get; private set; }

        /// <summary>
        /// Contructor which is called from factory. All necessary services are passed here.
        /// </summary>
        /// <param name="modelService"></param>
        /// <param name="connectionService"></param>
        /// <param name="eventService"></param>
        public ExtensionComponentViewModel(IModelService modelService, IConnectionService connectionService, IEventService eventService)
        {
            _modelService = modelService;
            _eventService = eventService;
            _connectionService = connectionService;

            InitializeEventList();

            InitializeFireButton();
        }

        private void InitializeFireButton()
        {
            FireEvent = new DelegateCommand(OnFireEvent, CanFireEvent);

            // assign to connection events 
            // update fire event button
            _connectionService.Connected += (sender, args) => FireEvent.RaiseCanExecuteChanged();
            _connectionService.Disconnected += (sender, args) => FireEvent.RaiseCanExecuteChanged();
        }

        private void InitializeEventList()
        {
            // check if monitor has a model loaded
            if (_modelService.ModelIsLoaded)
            {
                // read all events from the model
                Events = _modelService.Model.Events;
            }
            else
            {
                Events = new List<Event>();
            }
        }

        private bool CanFireEvent()
        {
            // fire button is only active if an event is selected and EB GUIDE Monitor is connected 
            return SelectedEvent != null && _connectionService.IsConnected;
        }

        private void OnFireEvent()
        {
            // Fire the selected events with help of IEventService
            _eventService.FireEvent(_selectedEvent, new List<ModelValue>());
        }

        public void Dispose()
        {
            // nothing here
        }
    }
}
