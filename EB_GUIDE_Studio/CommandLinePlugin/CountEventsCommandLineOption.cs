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


namespace CommandLinePlugin
{
    using System.ComponentModel.Composition;

    using Elektrobit.Guide.CommandLine;

    // Register this class at EB GUIDE's command line handler
    [Export(typeof(ICommandLineOptions))]
    // Register this class by its own type so that other classes can access the option.
    [Export(typeof(CountEventsCommandLineOption))]
    // Export this instance as a singleton
    [PartCreationPolicy(CreationPolicy.Shared)]
    internal class CountEventsCommandLineOption : ICommandLineOptions
    {
        public bool CountEvents { get; private set; }

        public void RegisterOptions(ICommandLineRegistration registration)
        {
            registration.Register(
                'x',
                "countEvents",
                "Count the number of events in the model",
                c => CountEvents = c);
        }
    }
}
