namespace ValidationPlugin
{
    using System.Collections.Generic;
    using System.ComponentModel.Composition;
    using System.Linq;
    using System.Threading;

    using Elektrobit.Guide.Studio.Model.Actions.Validation;
    using Elektrobit.Guide.Studio.Model.Elements;
    using Elektrobit.Guide.Studio.Model.Elements.Datapool;

    [Export(typeof(IModelValidator))]
    internal class FindItemsWithLanguageSupportValidator : IModelValidator
    {
        public IEnumerable<ModelProblem> Validate(IProjectContext projectContext, CancellationToken cancellationToken)
        {
            return projectContext.Project
                .GetAllDpItems()
                .Where(HasLanguageSupport)
                .Select(CreateProblem);
        }

        private static bool HasLanguageSupport(IDpItem datapoolItem)
        {
            return datapoolItem.IsLanguageDpItem();
        }

        private static ModelProblem CreateProblem(IDpItem datapoolItem)
        {
            return new ModelProblem(
                ModelProblemSeverity.Info,
                $"Datapool item {datapoolItem.Name} has language support enabled.",
                datapoolItem);
        }
    }
}
