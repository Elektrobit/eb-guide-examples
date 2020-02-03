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
