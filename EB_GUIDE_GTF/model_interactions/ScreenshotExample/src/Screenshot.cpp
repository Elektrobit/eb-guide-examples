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

#include "Screenshot.h"

#include <stdio.h>
#include <string.h>

#include <gtf/osal/CriticalSection.h>
#include <gtf/dependencyresolver/DependencyContainer.h>
#include <gtf/eventsystem/EventQueue.h>

namespace screenshot {

static void writeTGA(const char* filename, uint16_t width, uint16_t height, uint32_t bpp, void* data)
{
    unsigned char header[18] = { 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    FILE*         tgaFile    = fopen(filename, "wb");
    if (tgaFile)
    {
        header[12] = width & 0xFF;
        header[13] = (width >> 8) & 0xFF;
        header[14] = (height)&0xFF;
        header[15] = (height >> 8) & 0xFF;
        header[16] = (bpp == 4) ? 32 : 16;
        // write out the TGA header
        fwrite(header, 1, 18, tgaFile);
        //write out the data:
        fwrite(data, 1, width * height * bpp, tgaFile);
        fclose(tgaFile);
    }
}

Screenshot::Screenshot(uint32_t id)
    : active(false), sceneId(id)
{
}

Screenshot::Screenshot()
    : active(false)
    , sceneId(gtf::types::limits::uint32_max)
{
}

bool Screenshot::isActive()
{
    return active;
}

void Screenshot::flipImage(gtf::scenecapture::Bitmap const& bitmap)
{
    const uint32_t lineSourceSize      = bitmap.stride;
    const uint32_t lineDestinationSize = bitmap.width * bitmap.bytesPerPixel();

    for (uint32_t y = 0; y < bitmap.height; ++y)
    {
        const uint8_t* pixelSource      = bitmap.data + lineSourceSize * y;
        uint8_t*       pixelDestination = buffer.getData() + lineDestinationSize * (bitmap.height - y - 1);
        memcpy(pixelDestination, pixelSource, lineDestinationSize);
    }
}

void Screenshot::updateScene(const uint32_t sceneId_, gtf::scenecapture::Bitmap const& bitmap)
{
    if (active == false || sceneId_ != sceneId || bitmap.pixelFormat != gtf::scenecapture::Bitmap::PIXEL_FORMAT_4BYTE_RGBA_8888)
    {
        return;
    }

    gtf::osal::CriticalSection<gtf::osal::UserspaceMutex> lock(mutex);

    buffer.resize(bitmap.width * bitmap.height * bitmap.bytesPerPixel());
    flipImage(bitmap);
    writeTGA(filePath, bitmap.width, bitmap.height, bitmap.bytesPerPixel(), buffer.getData());

    active = false;
}

void Screenshot::activateScreenshot(const char* const path_)
{
    strncpy(filePath, path_, ARRAY_SIZE(filePath));
    active = true;
}
} // namespace screenshot
