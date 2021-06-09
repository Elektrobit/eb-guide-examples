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

namespace TGAImageLoaderPlugin.Decoders
{
    using System.Linq;
    using System.Windows.Media;

    public static class RgbDecoder
    {
        public static ImageSource Decode(byte[] data, TgaHeader tgaHeader, int size)
        {
            var imageDataOffset = RgbBufferDecoder.GetImageDataOffset(tgaHeader);

            var numberOfPixels = RgbBufferDecoder.GetStride(tgaHeader) * tgaHeader.Height;
            var imageBuffer = data.Skip(imageDataOffset).Take(numberOfPixels).ToArray();

            return RgbBufferDecoder.Decode(tgaHeader, imageBuffer, size);
        }
    }
}