namespace JsonImporterPlugin.Service
{
    using System.Threading.Tasks;

    using Elektrobit.Guide.Studio.Model.Elements;

    using JsonImporterPlugin.Model;

    /// <summary>
    /// Modifies the model according to imported Json file
    /// </summary>
    internal interface IJsonModelService
    {
        /// <summary>
        /// Task for importing and creating model elements from Json data.
        /// </summary>
        /// <param name="projectContext">The project context.</param>
        /// <param name="file">Selected Json file</param>
        /// <returns>The ImportModelDataResult.</returns>
        Task<ImportModelDataResult> ImportModelData(IProjectContext projectContext, string file);
    }
}
