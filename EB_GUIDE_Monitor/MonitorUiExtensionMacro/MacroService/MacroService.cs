////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Elektrobit Automotive GmbH
// Alle Rechte vorbehalten. All Rights Reserved.
// Information contained herein is subject to change without notice.
// Elektrobit retains ownership and all other rights in the software and each
// component thereof.
// Any reproduction of the software or components thereof without the prior
// written permission of Elektrobit is prohibited.
////////////////////////////////////////////////////////////////////////////////

namespace MonitorUiExtensionMacro.MacroService
{
    using System.Collections.Generic;
    using System.ComponentModel.Composition;
    using System.Linq;

    using Elektrobit.Guide.Monitor.Service;

    [Export(typeof(IMacroService))]
    public class MacroService : IMacroService
    {
        private readonly IConnectionService _connectionService;

        private readonly IEnumerable<ExportFactory<IMacroRecorder>> _recorderFactories;

        private readonly IMacroRecordSessionFactory _sessionFactory;

        [ImportingConstructor]
        public MacroService(
            IConnectionService connectionService,
            [ImportMany]IEnumerable<ExportFactory<IMacroRecorder>> recorderFactories,
            IMacroRecordSessionFactory sessionFactory)
        {
            _connectionService = connectionService;
            _recorderFactories = recorderFactories;
            _sessionFactory = sessionFactory;
        }

        public IEnumerable<IMacroRecorder> CreateRecorders()
        {
            return _recorderFactories.Select(factory => factory.CreateExport().Value);
        }

        public IMacroRecordSession CreateSession(IMacroRecorder recorder)
        {
            if (!_connectionService.IsConnected)
            {
                return null;
            }

            return _sessionFactory.Create(recorder);
        }
    }
}
