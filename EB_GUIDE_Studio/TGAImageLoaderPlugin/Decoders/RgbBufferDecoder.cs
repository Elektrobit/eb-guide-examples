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
    using System.Windows.Media.Imaging;

    public static class RgbBufferDecoder
    {
        public static ImageSource Decode(TgaHeader tgaHeader, byte[] imageBuffer, int size)
        {
            const double DPI_X = 96d;
            const double DPI_Y = 96d;
            var stride = GetStride(tgaHeader);
            var pixelFormat = GetPixelFormat(tgaHeader);

            var bitmap = BitmapSource.Create(
                tgaHeader.Width,
                tgaHeader.Height,
                DPI_X,
                DPI_Y,
                pixelFormat,
                null,
                imageBuffer,
                stride);


            var transformedBmp = new TransformedBitmap();
            transformedBmp.BeginInit();
            transformedBmp.Source = bitmap;
            transformedBmp.Transform = CreateShrinkTransform(tgaHeader, size);
            transformedBmp.EndInit();

            return transformedBmp;
        }

        private static ScaleTransform CreateShrinkTransform(TgaHeader tgaHeader, int size)
        {
            var scaleX = GetScaleX(tgaHeader, size);
            var scaleY = GetScaleY(tgaHeader, size);
            var transformedBmpTransform = new ScaleTransform(scaleX, scaleY);
            return transformedBmpTransform;
        }

        private static double GetScaleX(TgaHeader tgaHeader, int size)
        {
            return GetScale(tgaHeader, size, IsFlippedHorizontally);
        }

        private static double GetScaleY(TgaHeader tgaHeader, int size)
        {
            return GetScale(tgaHeader, size, IsFlippedVertically);
        }

        private static double GetScale(TgaHeader tgaHeader, int size, Func<TgaHeader, bool> isFlipped)
        {
            var divisor = Math.Max(tgaHeader.Width, tgaHeader.Height);
            var scaleY = (isFlipped(tgaHeader) ? -1 : 1) * ((double)size / divisor);
            return scaleY;
        }

        public static int GetStride(TgaHeader tgaHeader)
        {
            var bytesPerPixel = GetBytesPerPixel(tgaHeader);
            var stride = bytesPerPixel * tgaHeader.Width;
            return stride;
        }

        private static PixelFormat GetPixelFormat(TgaHeader tgaHeader)
        {
            switch (tgaHeader.BitsPerPixel)
            {
                case 24:
                    return PixelFormats.Bgr24;
                case 32:
                    return PixelFormats.Bgra32;
            }
            throw new UnsupportedPixelFormatException();
        }

        private static bool IsFlippedVertically(TgaHeader tgaHeader)
        {
            return (tgaHeader.ImageDescriptor & (1 << 5)) == 0;
        }

        private static bool IsFlippedHorizontally(TgaHeader tgaHeader)
        {
            return (tgaHeader.ImageDescriptor & (1 << 4)) != 0;
        }

        public static int GetBytesPerPixel(TgaHeader tgaHeader)
        {
            return (tgaHeader.BitsPerPixel + 7) / 8;
        }

        public static int GetImageDataOffset(TgaHeader tgaHeader)
        {
            int offset = tgaHeader.IdLength;
            offset += tgaHeader.ColorMapType == 0 ? 0 : GetColorMapLength(tgaHeader);
            return offset;
        }

        private static int GetColorMapLength(TgaHeader tgaHeader)
        {
            return tgaHeader.ColorMapLength * tgaHeader.ColorMapDepth / 8;
        }
    }
}