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

    using Elektrobit.Guide.Studio.Assets.ViewModels;
    using Elektrobit.Guide.Studio.Model.Asset;
    using Elektrobit.Guide.Studio.Model.Elements;

    [Export(typeof(IAssetViewModelFactory))]
    public class TgaAssetViewModelFactory : IAssetViewModelFactory
    {
        public IAssetFormat Format => TgaAssetFormats.Tga;

        public AssetViewModel CreateViewModel(IAsset asset, IProjectContext projectContext)
        {
            return new TgaAssetViewModel(asset, projectContext);
        }
    }
}