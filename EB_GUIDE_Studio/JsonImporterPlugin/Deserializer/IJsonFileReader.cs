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


namespace JsonImporterPlugin.Deserializer
{
    using JsonImporterPlugin.Model;

    /// <summary>
    /// Methods for selecting and reading Json file
    /// </summary>
    internal interface IJsonFileReader
    {
        /// <summary>
        /// Opens file dialog for selecting Json file.
        /// </summary>
        /// <param name="initialDir">The initial directory when selecting the file.</param>
        /// <returns>Path to selected Json file.</returns>
        string SelectJsonFile(string initialDir);

        /// <summary>
        /// Reads model data from selected Json file.
        /// </summary>
        /// <param name="file">Path to selected Json file.</param>
        /// <param name="msg">The possible parsing error message.</param>
        /// <returns>JsonModelData object.</returns>
        JsonModelData ReadModelDataFromJson(string file, out string msg);
    }
}
