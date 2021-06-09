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

#include "Mandelbrot2MemImageConverter.h"
#include <cmath>

Mandelbrot2MemImageConverter::Mandelbrot2MemImageConverter()
{
    for (uint32_t i = 0; i != rainbowGradientSize; ++i)
    {
        uint32_t red   = static_cast<uint8_t>(std::sin(0.03f * float(i) + 0.0f) * 127.0f + 128.0f);
        uint32_t blue  = static_cast<uint8_t>(std::sin(0.03f * float(i) + 2.0f) * 127.0f + 128.0f);
        uint32_t green = static_cast<uint8_t>(std::sin(0.03f * float(i) + 4.0f) * 127.0f + 128.0f);
        rainbow[i]     = green + (blue << 8) + (red << 16); // convert to RBG value
    }
}

void Mandelbrot2MemImageConverter::moveImageData(MandelbrotImage& mandelbrotImage, gtf::decoder::MemImage::Info& outMemImageInfo)
{
    for (uint32_t row = 0; row != mandelbrotImage.getHeight(); ++row)
    {
        moveImageRowData(mandelbrotImage, outMemImageInfo, row);
    }
}

void Mandelbrot2MemImageConverter::moveImageRowData(MandelbrotImage& mandelbrotImage, gtf::decoder::MemImage::Info& outMemImageInfo, uint32_t row)
{
    MandelbrotImage::value_type* srcRow = mandelbrotImage[row];

    uint8_t* destRow = outMemImageInfo.pixelData + outMemImageInfo.stride * row;

    for (uint32_t column = 0; column != mandelbrotImage.getWidth(); ++column)
    {
        moveImageColumnData(srcRow, destRow, column);
    }
}

void Mandelbrot2MemImageConverter::moveImageColumnData(MandelbrotImage::value_type* srcRow, uint8_t* destRow, uint32_t column)
{
    uint8_t& red   = destRow[column * 3 + 0];
    uint8_t& blue  = destRow[column * 3 + 1];
    uint8_t& green = destRow[column * 3 + 2];

    if (srcRow[column] <= 0.0)
    {
        red   = 0;
        blue  = 0;
        green = 0;
        return;
    }

    uint32_t const index = static_cast<uint32_t>(srcRow[column] * 50) % rainbowGradientSize;
    uint32_t const color = rainbow[index];
    red                  = static_cast<uint8_t>((color & 0xFF0000) >> 16);
    blue                 = static_cast<uint8_t>((color & 0xFF00) >> 8);
    green                = static_cast<uint8_t>(color & 0xFF);
}
