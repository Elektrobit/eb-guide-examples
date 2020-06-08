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


namespace ProjectMetadataPlugin
{
    using System;
    using System.ComponentModel.Composition;
    using System.IO;
    using System.Text;

    using Elektrobit.Guide.Studio.Model.Actions.Project;
    using Elektrobit.Guide.Studio.Model.Elements;
    using Elektrobit.Guide.Utilities;

    using Newtonsoft.Json;

    // Export as IProjectMetadataService to make the Metadata property available to users of the service
    [Export(typeof(IProjectMetadataService))]
    // Export as IProjectLifecycleListener to get notified about the project lifecycle
    [Export(typeof(IProjectLifecycleListener))]
    // Make sure this class is used as a singleton by MEF
    [PartCreationPolicy(CreationPolicy.Shared)]
    internal class ProjectMetadataService : IProjectLifecycleListener, IProjectMetadataService
    {
        private readonly IFileService _fileService;

        public IProjectMetadata Metadata { get; private set; }

        [ImportingConstructor]
        public ProjectMetadataService(IFileService fileService)
        {
            _fileService = fileService;
        }

        public void OnProjectCreated(IProjectContext projectContext)
        {
        }

        public void OnProjectOpened(IProjectContext projectContext)
        {
            var metadataFile = GetMetadataFile(projectContext);

            if (_fileService.FileExists(metadataFile) == false)
            {
                return;
            }

            using (var stream = _fileService.OpenExistingFileStream(metadataFile, FileAccess.Read, FileShare.Read))
            using (var reader = new StreamReader(stream, Encoding.UTF8))
            {
                var serializer = new JsonSerializer();
                var metadata = serializer.Deserialize(reader, typeof(ProjectMetadata)) as ProjectMetadata;

                Metadata = metadata;
            }
        }

        public void OnProjectSaved(IProjectContext projectContext)
        {
            var metadataFile = GetMetadataFile(projectContext);
            
            using (var stream = _fileService.CreateNewFileStream(metadataFile))
            using (var writer = new StreamWriter(stream, Encoding.UTF8))
            {

                var metadata = new ProjectMetadata
                                   {
                                       Name = projectContext.Name,
                                       Location = projectContext.Location,
                                       User = Environment.UserName,
                                       LastSavedDate = DateTime.Now
                                   };

                Metadata = metadata;

                var serializer = new JsonSerializer { Formatting = Formatting.Indented };
                serializer.Serialize(writer, metadata);

                writer.Flush();
            }
        }

        private static string GetMetadataFile(IProjectContext projectContext)
        {
            return Path.Combine(projectContext.Location, "metadata.json");
        }
    }
}
