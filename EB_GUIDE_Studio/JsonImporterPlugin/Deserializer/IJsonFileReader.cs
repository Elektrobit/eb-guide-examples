namespace JsonImporterPlugin.Deserializer
{
    using Elektrobit.Guide.Studio.Model.Elements;

    using JsonImporterPlugin.Model;

    /// <summary>
    /// Methods for selecting and reading Json file
    /// </summary>
    internal interface IJsonFileReader
    {
        /// <summary>
        /// Opens file dialog for selecting Json file.
        /// </summary>
        /// <param name="projectContext">The project context.</param>
        /// <returns>Path to selected Json file.</returns>
        string SelectJsonFile(IProjectContext projectContext);

        /// <summary>
        /// Reads model data from selected Json file.
        /// </summary>
        /// <param name="file">Path to selected Json file.</param>
        /// <param name="msg">The possible parsing error message.</param>
        /// <returns>JsonModelData object.</returns>
        JsonModelData ReadModelDataFromJson(string file, out string msg);
    }
}
