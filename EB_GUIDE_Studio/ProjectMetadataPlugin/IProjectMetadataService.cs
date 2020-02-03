namespace ProjectMetadataPlugin
{
    /// <summary>
    /// Service interface to access meta information stored with the project.
    /// Other classes can import this interface to gain access to the project metadata.
    /// </summary>
    public interface IProjectMetadataService
    {
        /// <summary>
        /// Gets the currently stored metadata.
        /// </summary>
        IProjectMetadata Metadata { get; }
    }
}