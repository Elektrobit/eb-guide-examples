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

#ifndef MANDELBROT_2_MEM_IMAGE_CONVERTER_H
#define MANDELBROT_2_MEM_IMAGE_CONVERTER_H

#include "MandelbrotImage.h"
#include <gtf/decoder/MemImage.h>

class Mandelbrot2MemImageConverter
{
public:
    Mandelbrot2MemImageConverter();

    void moveImageData(MandelbrotImage& mandelbrotImage, gtf::decoder::MemImage::Info& outMemImageInfo);

private:
    void moveImageRowData(MandelbrotImage& mandelbrotImage, gtf::decoder::MemImage::Info& outMemImageInfo, uint32_t row);

    void moveImageColumnData(MandelbrotImage::value_type* srcRow, uint8_t* destRow, uint32_t column);

    static const uint32_t rainbowGradientSize = 1024;
    uint32_t              rainbow[rainbowGradientSize];
};

#endif // MANDELBROT_2_MEM_IMAGE_CONVERTER_H
