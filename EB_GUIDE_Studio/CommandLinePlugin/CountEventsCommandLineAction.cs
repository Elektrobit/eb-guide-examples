namespace CommandLinePlugin
{
    using System.ComponentModel.Composition;
    using System.Linq;
    using System.Threading.Tasks;

    using Elektrobit.Guide.Studio.Model.Actions.Project.Commandline;
    using Elektrobit.Guide.Studio.Model.Elements;

    using log4net;

    [Export(typeof(IOpenProjectCommandLineAction))]
    internal class CountEventsCommandLineAction : IOpenProjectCommandLineAction
    {
        private static readonly ILog Logger = LogManager.GetLogger(typeof(CountEventsCommandLineAction));

        private readonly CountEventsCommandLineOption _option;

        [ImportingConstructor]
        public CountEventsCommandLineAction(CountEventsCommandLineOption option)
        {
            _option = option;
        }

        public Task<OpenProjectCommandLineActionResult> Execute(IProjectContext projectContext)
        {
            if (_option.CountEvents)
            {
                var count = projectContext.Project.GetAllEvents().Count();
                
                Logger.InfoFormat("Project {0} contains {1} events.", projectContext.Name, count);
            }

            return Task.FromResult(OpenProjectCommandLineActionResult.DoNothing);
        }
    }
}
