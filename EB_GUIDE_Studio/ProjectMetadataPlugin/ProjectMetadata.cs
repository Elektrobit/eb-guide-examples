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
