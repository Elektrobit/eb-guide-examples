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