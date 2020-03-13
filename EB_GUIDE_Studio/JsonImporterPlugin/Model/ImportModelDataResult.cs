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


namespace JsonImporterPlugin.Model
{
    using JsonImporterPlugin.Service;

    /// <summary>
    /// Result object returned by <see cref="IJsonModelService.ImportModelData"/>
    /// </summary>
    internal class ImportModelDataResult
    {
        private enum ImportModelDataStatus
        {
            Success,
            Failed
        }

        private readonly ImportModelDataStatus _status;

        public bool IsSuccess => _status == ImportModelDataStatus.Success;

        public string Title { get; }

        public string Message { get; }

        private ImportModelDataResult(ImportModelDataStatus status, string title, string msg)
        {
            _status = status;
            Title = title;
            Message = msg;
        }

        public static ImportModelDataResult Success(string msg)
        {
            return new ImportModelDataResult(ImportModelDataStatus.Success, "Import successful", msg);
        }

        public static ImportModelDataResult Failed(string msg)
        {
            return new ImportModelDataResult(ImportModelDataStatus.Failed, "Import failed", msg);
        }
    }
}
