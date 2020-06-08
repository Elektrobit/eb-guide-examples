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

#include <cmath>
#include "MandelbrotSet.h"
#include "MandelbrotImage.h"
#include "Mandelbrot2MemImageConverter.h"

MandelbrotSet::MandelbrotSet()
{
}

gtf::decoder::MemImage::Info MandelbrotSet::createImageInfo()
{
    gtf::decoder::MemImage::Info memImageInfo = gtf::decoder::MemImage::Info();
    memImageInfo.colorSpace                   = gtf::decoder::MemImage::ColorSpace::R8G8B8;
    memImageInfo.width                        = WIDTH;
    memImageInfo.height                       = HEIGHT;
    memImageInfo.stride                       = WIDTH * MULTIPLIER;

    memImageInfo.pitch = memImageInfo.width * memImageInfo.height * MULTIPLIER;

    return memImageInfo;
}

void MandelbrotSet::createMandelbrotImage(gtf::decoder::MemImage::Info& outMemImageInfo, uint32_t timeStep)
{
    const uint32_t stepOfReturn = 40;

    timeStep = timeStep % (2 * stepOfReturn);

    if (timeStep > stepOfReturn)
    {
        timeStep = stepOfReturn - (timeStep % stepOfReturn);
    }

    double const scale = 2.0 + std::pow(1.2, static_cast<double>(timeStep) / 2.0);

    MandelbrotSet::value_type const middleArcOfMandelbrotSetX = -0.15550494345;
    MandelbrotSet::value_type const middleArcOfMandelbrotSetY = 0.65017043818;

    MandelbrotImage mandelbrotImage(WIDTH, HEIGHT, middleArcOfMandelbrotSetX, middleArcOfMandelbrotSetY, scale, stepOfReturn);

    Mandelbrot2MemImageConverter converter;

    converter.moveImageData(mandelbrotImage, outMemImageInfo);
}
