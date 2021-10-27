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

#ifndef MANDELBROT_SET_H
#define MANDELBROT_SET_H

#include <gtf/types/TypesC99.h>
#include <gtf/decoder/MemImage.h>

class MandelbrotSet
{
public:
    typedef double value_type;
    MandelbrotSet();

    gtf::decoder::MemImage::Info createImageInfo();

    /*!
     * \brief Create a MandelbrotImage for the given \a timestep
     *
     * MandelbrotSet creates a zoom animation that zooms into a certain
     * point of the mandelbrot set.
     */
    void createMandelbrotImage(gtf::decoder::MemImage::Info& outMemImageInfo, uint32_t timeStep);

private:
    const uint32_t WIDTH      = 128;
    const uint32_t HEIGHT     = 128;
    const uint32_t MULTIPLIER = 3;
};

#endif // MANDELBROT_SET_H
