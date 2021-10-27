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
    using System;
    using System.Windows.Media;

    internal static class RgbRleDecoder
    {
        public static ImageSource Decode(byte[] data, TgaHeader tgaHeader, int size)
        {
            var imageBuffer = GetImageBuffer(data, tgaHeader);

            return RgbBufferDecoder.Decode(tgaHeader, imageBuffer, size);
        }

        private static byte[] GetImageBuffer(byte[] data, TgaHeader tgaHeader)
        {
            var imageDataOffset = RgbBufferDecoder.GetImageDataOffset(tgaHeader);
            var bytesPerPixel = RgbBufferDecoder.GetBytesPerPixel(tgaHeader);
            var pixelsToDecode = tgaHeader.Width * tgaHeader.Height;

            var outBuffer = new byte[pixelsToDecode * bytesPerPixel];


            for (int i = 0, inOffset = imageDataOffset, outOffset = 0; i < pixelsToDecode;)
            {
                var header = data[inOffset++];
                var pixelCount = GetPixelCount(header);

                if (IsRle(header))
                {
                    for (int count = 0; count < pixelCount; count++)
                    {
                        Array.Copy(data, inOffset, outBuffer, outOffset, bytesPerPixel); 
                        outOffset += bytesPerPixel;
                    }

                    inOffset += bytesPerPixel;
                }
                else
                {
                    var pixelsBytesLength = pixelCount * bytesPerPixel;
                    Array.Copy(data, inOffset, outBuffer, outOffset, pixelsBytesLength);

                    inOffset += pixelsBytesLength;
                    outOffset += pixelsBytesLength;

                }
                i += pixelCount;
            }

            return outBuffer;
        }

        private static int GetPixelCount(byte header)
        {
            return (header & 0x7F) + 1;
        }

        private static bool IsRle(byte header)
        {
            return (header & (1 << 7)) != 0;
        }
    }
}