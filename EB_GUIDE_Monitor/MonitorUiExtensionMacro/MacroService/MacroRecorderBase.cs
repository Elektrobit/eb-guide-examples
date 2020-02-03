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
    using System;
    using System.Collections.Generic;

    using Elektrobit.Guide.Utilities;

    public abstract class MacroRecorderBase
    {
        private readonly IDateTimeService _dateTimeService;

        private readonly bool _recordDelays;

        private long _lastAddTimeStamp;

        private readonly Queue<IMacroStep> _steps;

        protected IEnumerable<IMacroStep> Steps => _steps;

        protected MacroRecorderBase(IDateTimeService dateTimeService, bool recordDelays)
        {
            _dateTimeService = dateTimeService;
            _recordDelays = recordDelays;
            _steps = new Queue<IMacroStep>();
        }

        protected void AddStep(IMacroStep step)
        {
            if(_recordDelays)
            {
                RecordDelayStep();
            }

            _steps.Enqueue(step);
        }

        protected void ClearSteps()
        {
            _steps.Clear();
        }

        private void RecordDelayStep()
        {
            if (_steps.IsEmpty())
            {
                _lastAddTimeStamp = _dateTimeService.GetCurrentTimeStamp().Ticks;
            }
            else
            {
                var now = _dateTimeService.GetCurrentTimeStamp().Ticks;
                var diff = (now - _lastAddTimeStamp) / TimeSpan.TicksPerMillisecond;
                _lastAddTimeStamp = now;
                _steps.Enqueue(new MacroDelayStep(new Delay(diff)));
            }
        }
    }
}