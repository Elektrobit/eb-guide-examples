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
    using System;
    using System.ComponentModel.Composition;

    using Elektrobit.Guide.Studio.Model.Elements;
    using Elektrobit.Guide.Ui.Mvvm;
    using Elektrobit.Guide.Ui.Tools;
    using Elektrobit.Guide.Utilities;

    using JsonImporterPlugin.Model;

    using log4net;

    using Newtonsoft.Json;

    [Export(typeof(IJsonFileReader))]
    internal class JsonFileReader : IJsonFileReader
    {
        private static readonly ILog Logger = LogManager.GetLogger(typeof(JsonFileReader));

        private const string JSON_FILE_DIALOG_TITLE = "Select JSON file";

        private readonly IFileDialogManager _fileDialogManager;

        private readonly IFileService _fileService;

        private readonly IWindowManager _windowManager;

        [ImportingConstructor]
        public JsonFileReader(IFileDialogManager fileDialogManager, IFileService fileService, IWindowManager windowManager)
        {
            _fileDialogManager = fileDialogManager;
            _fileService = fileService;
            _windowManager = windowManager;
        }

        public string SelectJsonFile(IProjectContext projectContext)
        {
            using (var dialog = _fileDialogManager.CreateOpenFileDialog(
                JSON_FILE_DIALOG_TITLE,
                projectContext.Location,
                CreateJsonFileFilter()))
            {
                if (_windowManager.ShowDialog(dialog) == FileDialogResult.Ok)
                {
                    return dialog.SelectedFileName;
                }
            }

            return string.Empty;
        }

        public JsonModelData ReadModelDataFromJson(string file, out string msg)
        {
            JsonModelData data;
            msg = string.Empty;

            try
            {
                var json = _fileService.ReadAllText(file);
                data = JsonConvert.DeserializeObject<JsonModelData>(json);
            }
            catch (Exception ex)
            {
                Logger.Error($"Failed to read model data from '{file}'.", ex);
                data = null;
                msg = ex.Message;
            }

            return data;
        }

        private FileDialogFilter CreateJsonFileFilter()
        {
            return new FileDialogFilter(@"Json files", "*.json");
        }
    }
}
