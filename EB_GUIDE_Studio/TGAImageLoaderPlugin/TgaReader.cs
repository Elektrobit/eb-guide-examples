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
    using System;
    using System.IO;
    using System.Windows.Media;

    using TGAImageLoaderPlugin.Decoders;

    public class TgaReader
    {
        private readonly string _imagePath;


        public TgaReader(string imagePath)
        {
            _imagePath = imagePath;
        }

        public static TgaHeader ReadHeader(Stream stream)
        {
            var header = new TgaHeader
            {
                IdLength = ReadByte(stream),
                ColorMapType = ReadByte(stream),
                DataTypeCode = ReadDataTypeCode(stream),
                ColorMapOrigin = ReadShort(stream),
                ColorMapLength = ReadShort(stream),
                ColorMapDepth = ReadByte(stream),
                XOrigin = ReadShort(stream),
                YOrigin = ReadShort(stream),
                Width = ReadShort(stream),
                Height = ReadShort(stream),
                BitsPerPixel = ReadByte(stream),
                ImageDescriptor = ReadByte(stream)
            };

            return header;
        }

        public ImageSource Decode(int size)
        {
            using (var stream = new FileStream(_imagePath, FileMode.Open, FileAccess.Read, FileShare.Read))
            {
                return Decode(stream, size);
            }
        }

        private static ImageSource Decode(Stream stream, int size)
        {
            var tgaHeader = TgaReader.ReadHeader(stream);
            var dataLength = (int)stream.Length - System.Runtime.InteropServices.Marshal.SizeOf(tgaHeader);
            var data = ReadBytes(stream, dataLength);

            return Decode(data, tgaHeader, size);
        }

        private static ImageSource Decode(byte[] data, TgaHeader tgaHeader, int size)
        {
            switch (tgaHeader.DataTypeCode)
            {

                case DataTypeCode.Rgb:
                    return RgbDecoder.Decode(data, tgaHeader, size);
                case DataTypeCode.RgbRle:
                    return RgbRleDecoder.Decode(data, tgaHeader, size);
                case DataTypeCode.Bw:
                case DataTypeCode.BwRle:
                case DataTypeCode.ColorMapped:
                case DataTypeCode.ColorMappedRle:
                case DataTypeCode.ColorMappedCompressed:
                case DataTypeCode.ColorMappedCompressedQuad:
                case DataTypeCode.NoImageData:
                    return null; // We know about these formats but don't offer a preview of them
                default:
                    throw new UnsupportedPixelFormatException();
            }
        }

        private static DataTypeCode ReadDataTypeCode(Stream stream) => (DataTypeCode)ReadByte(stream);

        private static short ReadShort(Stream stream)
        {
            var bytes = ReadBytes(stream, 2);

            if (IsBigEndian())
            {
                Array.Reverse(bytes);
            }

            return BitConverter.ToInt16(bytes, 0);
        }

        private static bool IsBigEndian() => BitConverter.IsLittleEndian == false;

        private static byte ReadByte(Stream stream) => (byte)stream.ReadByte();

        private static byte[] ReadBytes(Stream stream, int length)
        {
            var data = new byte[length];
            stream.Read(data, 0, length);
            return data;
        }
    }
}