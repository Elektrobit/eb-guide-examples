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

#ifndef CUSTOM_WIDGET_KTX_TEXTURE_FORMAT_H_INCLUDED
#define CUSTOM_WIDGET_KTX_TEXTURE_FORMAT_H_INCLUDED

struct KTXTextureFormat
{
    enum Value
    {
        GL_ETC1_RGB8_OES                             = 0x8D64,
        GL_COMPRESSED_RGB8_ETC2                      = 0x9274,
        GL_COMPRESSED_SRGB8_ETC2                     = 0x9275,
        GL_COMPRESSED_RGBA8_ETC2_EAC                 = 0x9278,
        GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC          = 0x9279,
        GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2  = 0x9276,
        GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2 = 0x9277,
        GL_COMPRESSED_R11_EAC                        = 0x9270,
        GL_COMPRESSED_SIGNED_R11_EAC                 = 0x9271,
        GL_COMPRESSED_RG11_EAC                       = 0x9272,
        GL_COMPRESSED_SIGNED_RG11_EAC                = 0x9273,
        GL_RGBA8                                     = 0x8058,
        GL_RGBA8UI                                   = 0x8D7C,
        GL_SRGB8_ALPHA8                              = 0x8C43,
        GL_RGB8                                      = 0x8051,
        GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG           = 0x8C01,
        GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG           = 0x8C00,
        GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG          = 0x8C03,
        GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG          = 0x8C02,
        GL_COMPRESSED_RGBA_PVRTC_2BPPV2_IMG          = 0x9137,
        GL_COMPRESSED_RGBA_PVRTC_4BPPV2_IMG          = 0x9138,
        GL_COMPRESSED_RGBA_ASTC_4x4_KHR              = 0x93B0,
        GL_COMPRESSED_RGBA_ASTC_5x4_KHR              = 0x93B1,
        GL_COMPRESSED_RGBA_ASTC_5x5_KHR              = 0x93B2,
        GL_COMPRESSED_RGBA_ASTC_6x5_KHR              = 0x93B3,
        GL_COMPRESSED_RGBA_ASTC_6x6_KHR              = 0x93B4,
        GL_COMPRESSED_RGBA_ASTC_8x5_KHR              = 0x93B5,
        GL_COMPRESSED_RGBA_ASTC_8x6_KHR              = 0x93B6,
        GL_COMPRESSED_RGBA_ASTC_8x8_KHR              = 0x93B7,
        GL_COMPRESSED_RGBA_ASTC_10x5_KHR             = 0x93B8,
        GL_COMPRESSED_RGBA_ASTC_10x6_KHR             = 0x93B9,
        GL_COMPRESSED_RGBA_ASTC_10x8_KHR             = 0x93BA,
        GL_COMPRESSED_RGBA_ASTC_10x10_KHR            = 0x93BB,
        GL_COMPRESSED_RGBA_ASTC_12x10_KHR            = 0x93BC,
        GL_COMPRESSED_RGBA_ASTC_12x12_KHR            = 0x93BD,
        GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR      = 0x93D0,
        GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x4_KHR      = 0x93D1,
        GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR      = 0x93D2,
        GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x5_KHR      = 0x93D3,
        GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR      = 0x93D4,
        GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x5_KHR      = 0x93D5,
        GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x6_KHR      = 0x93D6,
        GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR      = 0x93D7,
        GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x5_KHR     = 0x93D8,
        GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR     = 0x93D9,
        GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x8_KHR     = 0x93DA,
        GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR    = 0x93DB,
        GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x10_KHR    = 0x93DC,
        GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR    = 0x93DD,
        GL_COMPRESSED_RGB_S3TC_DXT1_EXT              = 0x83F0,
        GL_COMPRESSED_RGBA_S3TC_DXT1_EXT             = 0x83F1,
        GL_COMPRESSED_RGBA_S3TC_DXT3_EXT             = 0x83F2,
        GL_COMPRESSED_RGBA_S3TC_DXT5_EXT             = 0x83F3,
        GL_COMPRESSED_SRGB_S3TC_DXT1_EXT             = 0x8C4C,
        GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT       = 0x8C4D,
        GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT       = 0x8C4E,
        GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT       = 0x8C4F
    };
};
#endif
