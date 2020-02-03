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
    using Elektrobit.Guide.Monitor.Service.Datapool;
    using Elektrobit.Guide.Monitor.Ui.Docking;

    [Export(typeof(IDockableComponentFactory))]
    internal class ExtensionViewModelFactory : IDockableComponentFactory
    {
        private readonly IModelService _modelService;

        private readonly IDatapoolService _datapoolService;

        private readonly IConnectionService _connectionService;

        public string ContentIdBase { get; }

        public string Title { get; }

        /// <summary>
        /// Factory imports all necessary services from MEF environment
        /// </summary>
        /// <param name="modelService">Model Service</param>
        /// <param name="connectionService">Connection Service</param>
        /// <param name="datapoolService">Datapool Service</param>
        [ImportingConstructor]
        public ExtensionViewModelFactory(IModelService modelService, IConnectionService connectionService, IDatapoolService datapoolService)
        {
            Title = "Monitor Datapool Extension";
            ContentIdBase = "monitor_extension_datapool_id";

            _modelService = modelService;
            _datapoolService = datapoolService;
            _connectionService = connectionService;
        }

        public IDockableComponentViewModel CreateComponent()
        {
            return new ExtensionComponentViewModel(_modelService, _connectionService, _datapoolService);
        }
    }
}
