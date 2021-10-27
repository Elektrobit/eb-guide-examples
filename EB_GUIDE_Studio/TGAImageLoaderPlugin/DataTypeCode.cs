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
    public enum DataTypeCode : byte
    {
        NoImageData = 0,
        ColorMapped = 1,
        Rgb = 2,
        Bw = 3, 
        ColorMappedRle = 9,
        RgbRle = 10,
        BwRle = 11,
        ColorMappedCompressed = 32,
        ColorMappedCompressedQuad = 33
    }
}