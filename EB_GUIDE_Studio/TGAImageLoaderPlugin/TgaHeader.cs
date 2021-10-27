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
    using System.Runtime.InteropServices;

    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    public struct TgaHeader
    {
        public byte IdLength;
        public byte ColorMapType;
        public DataTypeCode DataTypeCode;
        public short ColorMapOrigin;
        public short ColorMapLength;
        public byte ColorMapDepth;
        public short XOrigin;
        public short YOrigin;
        public short Width;
        public short Height;
        public byte BitsPerPixel;
        public byte ImageDescriptor;
    };
}