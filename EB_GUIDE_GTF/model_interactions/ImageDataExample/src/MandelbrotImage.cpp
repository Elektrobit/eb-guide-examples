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

#include <stdlib.h>
#include <complex>
#include "MandelbrotImage.h"

MandelbrotImage::MandelbrotImage(uint32_t widthToUse
    , uint32_t heightToUse
    , value_type posX
    , value_type posY
    , value_type scale
    , uint32_t maxIterations)
    : width(widthToUse)
    , height(heightToUse)
    , pixelCount(width*height)
{
    if (pixelCount == 0)
    {
        pixelData = NULL;
        return;
    }
    pixelData = new value_type[pixelCount];
    if (pixelData == NULL)
    {
        return;
    }

    drawMandelbrot(posX - value_type(widthToUse) / scale, posX + value_type(widthToUse) / scale
        , posY - value_type(heightToUse) / scale, posY + value_type(heightToUse) / scale
        , maxIterations
    );
}

MandelbrotImage::~MandelbrotImage()
{
    delete[] pixelData;
}

void MandelbrotImage::drawMandelbrot(value_type cxmin
    , value_type cxmax
    , value_type cymin
    , value_type cymax
    , uint32_t maxIterations)
{
    uint32_t const imageWidth = getWidth();
    uint32_t const imageHeight = getHeight();
    value_type scaleX = 1.0 / (value_type(imageWidth) - 1.0) * (cxmax - cxmin);
    value_type scaleY = 1.0 / (value_type(imageHeight) - 1.0) * (cymax - cymin);
    value_type escapeRadius = 20.0f;
    value_type log_2 = std::log(2.0f);
    value_type * pImage = pixelData;

    for (uint32_t row = 0; row < imageHeight; ++row)
    {
        value_type scaledImageY = cymin + value_type(row) * scaleY;
        for (uint32_t column = 0; column < imageWidth; ++column)
        {
            value_type scaledImageX = cxmin + value_type(column) * scaleX;
            std::complex<value_type> c(scaledImageX, scaledImageY);
            std::complex<value_type> z = 0;
            uint32_t iterations;

            for (iterations = 0
                ; iterations < maxIterations && abs(z) < escapeRadius
                ; ++iterations)
            {
                z = z * z + c;
            }

            *pImage++ = value_type(iterations) - (std::log(std::log(abs(z)) / log_2));
        }
    }
}
