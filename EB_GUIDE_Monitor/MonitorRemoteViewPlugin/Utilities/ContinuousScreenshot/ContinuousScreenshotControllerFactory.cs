////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Elektrobit Automotive GmbH
// Alle Rechte vorbehalten. All Rights Reserved.
// Information contained herein is subject to change without notice.
// Elektrobit retains ownership and all other rights in the software and each
// component thereof.
// Any reproduction of the software or components thereof without the prior
// written permission of Elektrobit is prohibited.
////////////////////////////////////////////////////////////////////////////////

namespace MonitorRemoteViewPlugin.Utilities.ContinuousScreenshot
{
    using System.ComponentModel.Composition;
    using Elektrobit.Guide.Monitor.Service.ScreenshotService;
    using MonitorRemoteViewPlugin.Utilities.Cancellation;
    using MonitorRemoteViewPlugin.Utilities.Delayer;

    [Export(typeof(IContinuousScreenshotControllerFactory))]
    public class ContinuousScreenshotControllerFactory : IContinuousScreenshotControllerFactory
    {
        private readonly IScreenshotService _screenshotService;
        private readonly IDelayer _delayer;

        [ImportingConstructor]
        public ContinuousScreenshotControllerFactory(IScreenshotService screenshotService, IDelayer delayer)
        {
            _screenshotService = screenshotService;
            _delayer = delayer;
        }

        public IContinuousScreenshotController CreateController()
        {
            var runner = new ContinuousScreenshotRunner(new Cancellation(), _screenshotService, _delayer);

            return new ContinuousScreenshotController(runner);
        }
    }
}