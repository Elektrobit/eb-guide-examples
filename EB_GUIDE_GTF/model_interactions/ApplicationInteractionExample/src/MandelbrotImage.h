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

#ifndef MANDELBROT_IMAGE_H
#define MANDELBROT_IMAGE_H

#include <gtf/types/TypesC99.h>

/**
 * \brief MandelbrotImage is an image type
 * used for storing the normalized number of iterations to
 * evaluate whether or not a pixel within a rectangular
 * region is part of the mandelbrot set.
 *
 * The iterations are calculated within the constructor
 */
class MandelbrotImage
{
public:
    typedef double value_type;

    MandelbrotImage(uint32_t width, uint32_t height, value_type posX, value_type posY, value_type scale, uint32_t maxIterations);

    ~MandelbrotImage();

    uint32_t getWidth() const
    {
        return width;
    }

    uint32_t getHeight() const
    {
        return height;
    }

    value_type* operator[](uint32_t row) const
    {
        if (NULL == pixelData)
        {
            return NULL;
        }
        return pixelData + row * width;
    }

private:
    void drawMandelbrot(value_type cxmin, value_type cxmax, value_type cymin, value_type cymax, uint32_t maxIterations);

    uint32_t    width;
    uint32_t    height;
    uint32_t    pixelCount;
    value_type* pixelData;
};

#endif // MANDELBROT_IMAGE_H
