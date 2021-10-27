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

    using Elektrobit.Guide.Studio.Model.Asset;
    using Elektrobit.Guide.Studio.Model.Elements.Resources;

    public static class TgaAssetFormats
    {
        [Export]
        public static IAssetFormat Tga = new AssetFormat(DefaultResourceTypes.Image, new []{".tga"});
    }
}