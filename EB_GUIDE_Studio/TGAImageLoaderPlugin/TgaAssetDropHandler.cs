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

namespace TGAImageLoaderPlugin
{
    using System.ComponentModel.Composition;
    using System.Drawing;
    using System.IO;

    using Elektrobit.Guide.Scheduler;
    using Elektrobit.Guide.Studio.Assets.ViewModels;
    using Elektrobit.Guide.Studio.Model.Actions.Widget;
    using Elektrobit.Guide.Studio.Model.Asset;
    using Elektrobit.Guide.Studio.Model.Elements;
    using Elektrobit.Guide.Studio.Model.Elements.Resources;
    using Elektrobit.Guide.Studio.Model.Elements.Value;
    using Elektrobit.Guide.Studio.Model.Elements.Widget;
    using Elektrobit.Guide.Studio.TargetTypes;
    using Elektrobit.Guide.Studio.Tools.DnD;
    using Elektrobit.Guide.Studio.Workbench;

    using LanguageExt;

    using Image = Elektrobit.Guide.Studio.Model.Elements.Resources.Image;

    [Export(typeof(IModelElementDropHandler))]
    public class TgaAssetDropHandler : ImageAssetDropHandler
    {
        protected override IAssetFormat AssetFormat => TgaAssetFormats.Tga;

        [ImportingConstructor]
        public TgaAssetDropHandler(IWidgetService widgetService, ITaskSchedulerProvider schedulerProvider)
            : base(widgetService, schedulerProvider)
        {
        }

        protected override Size GetImageSize(IProjectContext projectContext, Option<string> assetFile)
        {
            var header = assetFile.Bind(ReadHeader);
            return header.Match(
                Some: h =>new Size(h.Width, h.Height),
                None: () => new Size(0, 0));
        }

        private static Option<TgaHeader> ReadHeader(string fileName)
        {
            try
            {
                using (var stream = new FileStream(fileName, FileMode.Open))
                {
                    return TgaReader.ReadHeader(stream);
                }
            }
            catch
            {
                return Option<TgaHeader>.None;
            }
        }
    }
}
