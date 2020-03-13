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


namespace ExportStatisticsPlugin
{
    using System.ComponentModel.Composition;
    using System.IO;
    using System.Text;

    using Elektrobit.Guide.Studio.Exporter;
    using Elektrobit.Guide.Studio.Exporter.Model;

    [Export(typeof(IRuntimeModelPartPrinter))]
    internal class ExportStatisticsPrinter : IRuntimeModelPartPrinter
    {
        public void Save(
            ExportConfiguration exportConfiguration,
            ProjectExport projectExport,
            IExportDataWriter dataWriter)
        {
            // Only write the statistics file if the full model is exported.
            // We don't want to write the file for internal edit mode exports.
            if (exportConfiguration.Mode != ExportMode.FullModel)
            {
                return;
            }

            using (var stream = dataWriter.OpenStream("ExportStatistics.txt"))
            using (var writer = new StreamWriter(stream, Encoding.UTF8))
            {
                WriteStatistics(projectExport, writer);
                writer.Flush();
            }
        }

        private static void WriteStatistics(ProjectExport projectExport, TextWriter writer)
        {
            writer.WriteLine("Export statistics for project {0}:", projectExport.ProjectContext.Name);
            writer.WriteLine("- Events:         {0}", projectExport.Events.Count);
            writer.WriteLine("- Datapool items: {0}", projectExport.DpItems.Count);
            writer.WriteLine("- Languages:      {0}", projectExport.Languages.Count);
            writer.WriteLine("- Skins:          {0}", projectExport.Skins.Count);
            writer.WriteLine("- State machines: {0}", projectExport.StateMachines.Count);
            writer.WriteLine("- Dialogs:        {0}", projectExport.Dialogs.Count);
            writer.WriteLine("- Resources:      {0}", projectExport.Resources.Count);
        }
    }
}
