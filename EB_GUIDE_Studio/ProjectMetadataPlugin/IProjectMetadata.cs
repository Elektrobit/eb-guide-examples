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