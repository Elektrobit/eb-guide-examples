////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Elektrobit Automotive GmbH
// Alle Rechte vorbehalten. All Rights Reserved.
// Information contained herein is subject to change without notice.
// Elektrobit retains ownership and all other rights in the software and each
// component thereof.
// Any reproduction of the software or components thereof without the prior
// written permission of Elektrobit is prohibited.
////////////////////////////////////////////////////////////////////////////////

using Elektrobit.Guide.Ui.Mvvm;

namespace MonitorUiExtension.ViewModels
{
    using System.Collections.Generic;
    using System.Linq;

    using Elektrobit.Guide.Monitor.Service;
    using Elektrobit.Guide.Monitor.Service.Datapool;
    using Elektrobit.Guide.Monitor.Ui.Docking;

    using Prism.Commands;
    using Prism.Mvvm;

    internal class ExtensionComponentViewModel : ViewModel, IDockableComponentViewModel
    {
        private readonly IModelService _modelService;

        private readonly IDatapoolService _datapoolService;

        private readonly IConnectionService _connectionService;

        /// <summary>
        /// Binding property for view with list of all datapool items
        /// </summary>
        public List<VisibleValueViewModel> ValueViewModelList { get; set; }

        /// <summary>
        /// Binding command for reaction if subscription is started
        /// </summary>
        public DelegateCommand StartSubscription { get; private set; }

        /// <summary>
        /// Binding command for reaction if subscription is started is stopped
        /// </summary>
        public DelegateCommand StopSubscription { get; private set; }

        private bool _observing;

        /// <summary>
        /// Contructor which is called from factory. All necessary services are passed here.
        /// </summary>
        /// <param name="modelService"></param>
        /// <param name="connectionService"></param>
        /// <param name="datapoolService"></param>
        public ExtensionComponentViewModel(IModelService modelService, IConnectionService connectionService, IDatapoolService datapoolService)
        {
            _modelService = modelService;
            _datapoolService = datapoolService;
            _connectionService = connectionService;

            _observing = false;

            InitializeDatapoolList();

            InitializeCommands();
        }

        private void InitializeCommands()
        {
            StartSubscription = new DelegateCommand(OnStartSubscription, CanStartSubscription);

            StopSubscription = new DelegateCommand(OnStopSubscription, CanStopSubscription);

            // assign to connection events 
            // update fire event button
            _connectionService.Connected += (sender, args) =>
                {
                    UpdateCommands();
                };
            _connectionService.Disconnected += (sender, args) =>
                {
                    UpdateCommands();
                };
        }

        private void UpdateCommands()
        {
            StartSubscription.RaiseCanExecuteChanged();
            StopSubscription.RaiseCanExecuteChanged();
        }

        private void InitializeDatapoolList()
        {
            ValueViewModelList = new List<VisibleValueViewModel>();

            // check if monitor has a model loaded
            if (!_modelService.ModelIsLoaded)
            {
                return;
            }

            // get module object from configuration (monitor.cfg)
            var module = _modelService.Model.Datapool.Modules.FirstOrDefault();
            if (module == null)
            {
                return;
            }

            // add all datapool items from the model to visible list
            foreach (var item in module.Items)
            {
                ValueViewModelList.Add(new VisibleValueViewModel(_datapoolService, item));
            }
        }

        private bool CanStartSubscription()
        {
            // subscribe button is only active if EB GUIDE Monitor is connected and not already observing
            return _connectionService.IsConnected && !_observing;
        }

        private async void OnStartSubscription()
        {
            _observing = true;

            foreach (var currViewModel in ValueViewModelList)
            {
                await currViewModel.Subscribe();
            }

            UpdateCommands();
        }

        private bool CanStopSubscription()
        {
            return _connectionService.IsConnected && _observing;
        }

        private async void OnStopSubscription()
        {
            _observing = false;

            foreach (var item in ValueViewModelList)
            {
                await item.Unsubscribe();
            }

            UpdateCommands();
        }

        public void Dispose()
        {
            OnStopSubscription();
        }
    }
}
