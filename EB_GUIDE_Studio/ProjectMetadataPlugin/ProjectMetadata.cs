namespace ProjectMetadataPlugin
{
    using System;

    /// <summary>
    /// Implementation of <see cref="IProjectMetadata"/> that is used for serialization.
    /// </summary>
    internal class ProjectMetadata : IProjectMetadata
    {
        public string Location { get; set; }

        public string Name { get; set; }

        public string User { get; set; }

        public DateTime LastSavedDate { get; set; }
    }
}
