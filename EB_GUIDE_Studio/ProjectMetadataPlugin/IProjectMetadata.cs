namespace ProjectMetadataPlugin
{
    using System;

    /// <summary>
    /// Read-only interface that provides project metadata information.
    /// </summary>
    public interface IProjectMetadata
    {
        string Location { get; }

        string Name { get; }

        string User { get; }

        DateTime LastSavedDate { get; }
    }
}