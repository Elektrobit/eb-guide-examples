////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Elektrobit Automotive GmbH
// Alle Rechte vorbehalten. All Rights Reserved.
// Information contained herein is subject to change without notice.
// Elektrobit retains ownership and all other rights in the software and each
// component thereof.
// Any reproduction of the software or components thereof without the prior
// written permission of Elektrobit is prohibited.
////////////////////////////////////////////////////////////////////////////////

namespace MonitorRemoteViewPlugin.Utilities.FrameDelayConverter
{
    using System.ComponentModel.Composition;

    [Export(typeof(IFrameDelayConverter))]
    public class FrameDelayConverter : IFrameDelayConverter
    {
        private const uint MILLISECONDS_IN_A_SECOND = 1000;
        private readonly uint _defaultDelay;

        public FrameDelayConverter() : this(MILLISECONDS_IN_A_SECOND)
        {
        }

        public FrameDelayConverter(uint defaultDelay)
        {
            _defaultDelay = defaultDelay;
        }

        public uint Convert(uint framesPerSecond)
        {
            if (framesPerSecond == 0)
            {
                return _defaultDelay;
            }

            return MILLISECONDS_IN_A_SECOND / framesPerSecond;
        }
    }
}