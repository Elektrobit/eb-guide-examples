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

#include "DecoderImpl.h"
#include "Tga.h"
#include <gtf/decoder/EncodedMemImage.h>
#include <gtf/scenecapture/Bitmap.h>
#include <ctype.h>

namespace  tgaimageloader
{
    DecoderImpl::DecoderImpl(const gtf::resourcesystem::FormatHandle& inputFormat_, const gtf::resourcesystem::FormatHandle& outputFormat_, const gtf::resourcesystem::FactoryHandle& factory_)
        :inputFormat(inputFormat_), outputFormat(outputFormat_), factory(factory_)
    {
    }

    gtf::resourcesystem::DataHandle DecoderImpl::decode(const gtf::resourcesystem::ResourceHandle& resource, const gtf::resourcesystem::DataHandle& data) const
    {
        if (!resource || !data)
        {
            return gtf::resourcesystem::DataHandle();
        }

        gtf::decoder::EncodedMemImage inputData(data);
        gtf::decoder::EncodedMemImage::BufferHandle inputBuffer = inputData.getData();

        if (!inputBuffer)
        {
            return gtf::resourcesystem::DataHandle();
        }

        const char* path = inputData.getPath();

        if (!path)
        {
            return gtf::resourcesystem::DataHandle();
        }

        const uint32_t pathLength = uint32_t(strlen(path) + 1);
        const char     tgaEnding[] = {'.', 't', 'g', 'a', '\0'};

        if (!matchStringEnd(path, pathLength, tgaEnding, ARRAY_SIZE(tgaEnding)))
        {
            return gtf::resourcesystem::DataHandle();
        }

        Tga tga;

        gtf::decoder::MemImage::Info info = gtf::decoder::MemImage::Info();


        if (!tga.getInfo(inputBuffer->getData(), inputBuffer->getSize(), info))
        {
            return gtf::resourcesystem::DataHandle();
        }
        gtf::resourcesystem::DataHandle result = factory->createData(outputFormat);
        if (!result)
        {
            return gtf::resourcesystem::DataHandle();
        }

        gtf::decoder::MemImage outputData(result);

        gtf::resourcesystem::CacheHandle cache = resource->getCache();
        if (!cache->canAllocateData(static_cast<uint32_t>(strlen(path))))
        {

            return gtf::resourcesystem::DataHandle();
        }

        if (!outputData.copyPath(inputData))
        {
            return gtf::resourcesystem::DataHandle();
        }

        if (!cache->canAllocateData(info.pitch))
        {

            return gtf::resourcesystem::DataHandle();
        }

        info.pixelData = static_cast<uint8_t*>(outputData.allocData(info.pitch));

        if (!info.pixelData || !tga.getData(inputBuffer->getData(), inputBuffer->getSize(), info))
        {
            return gtf::resourcesystem::DataHandle();
        }

        if (!cache->canAllocateData(sizeof(info)))
        {
            return gtf::resourcesystem::DataHandle();
        }

        if (!outputData.setInfo(info))
        {
            return gtf::resourcesystem::DataHandle();
        }

        return result;
    }

    bool DecoderImpl::matchStringEnd(const char * path, uint32_t pathLength, const char * match, uint32_t matchLength)
    {
        if (!path || !match || pathLength < matchLength)
        {
            return false;
        }

        for (uint32_t i = 0; i < matchLength; ++i)
        {
            const char* currentPath = path + pathLength - matchLength + i;
            const char* currentMatch = match + i;

            if (*currentPath == *currentMatch)
            {
                continue;
            }

            if (tolower(*currentPath) == tolower(*currentMatch))
            {
                continue;
            }

            return false;
        }

        return true;
    }
    gtf::resourcesystem::FormatHandle DecoderImpl::getInputFormat() const

    {
        return inputFormat;
    }

    gtf::resourcesystem::FormatHandle DecoderImpl::getOutputFormat() const
    {
        return outputFormat;
    }
}


