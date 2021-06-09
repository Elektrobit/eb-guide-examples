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

#include "Tga.h"
#include <gtf/decoder/EncodedMemImage.h>
#include <gtf/scenecapture/Bitmap.h>

namespace  tgaimageloader
{
    Tga::Tga()
        :headerData{0}, imageCompressed(false), isImageReversed(false), bytesPerPixel(0)
    {
    }

    Tga::~Tga()
    {
    }

    bool Tga::getInfo(const void* srcData, uint32_t srcSize, gtf::decoder::MemImage::Info& info)
    {
        GTF_UNUSED_PARAM(srcSize);

        uint32_t bitsPerPixel = 0;

        memcpy(headerData, srcData, ARRAY_SIZE(headerData));

        bitsPerPixel = headerData[16];
        if ((bitsPerPixel != 24) && (bitsPerPixel != 32))
        {
            return false;
        }

        bytesPerPixel = bitsPerPixel / 8;

        if (headerData[2] == 0xA)
        {
            imageCompressed = true;
        }

        isImageReversed = (headerData[17] & 0x20) == 0;

        info.width = headerData[13] * 256 + headerData[12];
        info.height = headerData[15] * 256 + headerData[14];

        if (bytesPerPixel == 4)
        {
            info.colorSpace = gtf::decoder::MemImage::ColorSpace::R8G8B8A8;
        }
        else
        {
            if(bytesPerPixel == 3)
            {
                info.colorSpace = gtf::decoder::MemImage::ColorSpace::R8G8B8;
            }
        }

        info.stride = bytesPerPixel * info.width;
        info.pitch = info.stride * info.height;
        info.trivialAlpha = false;

        return true;
    }

    void Tga::flipImage(gtf::decoder::MemImage::Info& info)
    {
        for (uint32_t j = 0; j * 2 < info.height; ++j)
        {
            int index1 = j * info.width * bytesPerPixel;
            int index2 = (info.height - 1 - j) * info.width * bytesPerPixel;
            for (uint32_t i = info.width * bytesPerPixel; i > 0; --i)
            {
                unsigned char temp = info.pixelData[index1];
                info.pixelData[index1] = info.pixelData[index2];
                info.pixelData[index2] = temp;
                ++index1;
                ++index2;
            }
        }
    }

    bool Tga::getData(const void* srcData, uint32_t srcSize, gtf::decoder::MemImage::Info& info)
    {
        gtf::container::Array<uint8_t>imageData;

        uint32_t size = srcSize - ARRAY_SIZE(headerData);

        imageData.appendArray(reinterpret_cast<const uint8_t*>(srcData) + ARRAY_SIZE(headerData), size);

        if (imageCompressed)
        {
            decodeCompressed(imageData.getData(), info.pixelData, info);
        }
        else
        {
            decodeUncompressed(imageData.getData(), info.pixelData, info);
        }
        if (isImageReversed)
        {
            flipImage(info);
        }

        return true;
    }

    void Tga::decodeUncompressed(uint8_t * inBuffer, uint8_t * outBuffer, gtf::decoder::MemImage::Info& info)
    {
        size_t size = info.height*info.width;
        TransferPixels(inBuffer, outBuffer, size);
    }

    void Tga::TransferPixels(uint8_t *&inBuffer, uint8_t *&outBuffer, size_t numberOfPixels)
    {
        for(uint32_t i = 0; i < numberOfPixels; ++i)
        {
            if(bytesPerPixel == 4)
            {
                TransferPixel32(inBuffer, outBuffer);
            }
            else
            {
                TransferPixel24(inBuffer, outBuffer);
            }
        }
    }

    void Tga::decodeCompressed(uint8_t * inBuffer, uint8_t * outBuffer, gtf::decoder::MemImage::Info& info)
    {
        uint8_t header;
        uint32_t pixelCount;
        uint8_t red, green, blue, alpha = 255;
        size_t size = info.height*info.width;

        for (uint32_t i = 0; i < size; )
        {
            header = *inBuffer++;
            pixelCount = (header & 0x7F) + 1;

            if (header & 0x80)
            {
                if(bytesPerPixel == 4)
                {
                    readPixel32(inBuffer, red, green, blue, alpha);
                }
                else
                {
                    readPixel24(inBuffer, red, green, blue);
                }

                if(bytesPerPixel == 4)
                {
                    writePixel32Multiple(outBuffer, pixelCount, red, green, blue, alpha);
                }
                else
                {
                    writePixel24Multiple(outBuffer, pixelCount, red, green, blue);
                }
            }
            else
            {
               TransferPixels(inBuffer, outBuffer, pixelCount);
            }
            i += pixelCount;
        }
    }

    void Tga::writePixel32Multiple(uint8_t *&outBuffer, size_t numberOfTimes, uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha)
    {
        for (uint32_t j = 0; j < numberOfTimes; j++)
        {
            writePixel32(outBuffer, red, green, blue, alpha);
        }
    }

    void Tga::writePixel24Multiple(uint8_t *&outBuffer, size_t numberOfTimes, uint8_t red, uint8_t green, uint8_t blue)
    {
        for (uint32_t j = 0; j < numberOfTimes; j++)
        {
            writePixel24(outBuffer, red, green, blue);
        }
    }
    void Tga::TransferPixel24(uint8_t *&inBuffer, uint8_t *&outBuffer) {
        uint8_t red, green, blue;
        readPixel24(inBuffer, red, green, blue);
        writePixel24(outBuffer, red, green, blue);
    }

    void Tga::TransferPixel32(uint8_t *&inBuffer, uint8_t *&outBuffer) {
        uint8_t red, green, blue, alpha;
        readPixel32(inBuffer, red, green, blue, alpha);
        writePixel32(outBuffer, red, green, blue, alpha);
    }

    void Tga::readPixel24(uint8_t *& buffer, uint8_t & red, uint8_t & green, uint8_t & blue)
    {
        blue = *buffer++;
        green = *buffer++;
        red =  *buffer++;
    }

    void Tga::readPixel32(uint8_t *& buffer, uint8_t & red, uint8_t & green, uint8_t & blue, uint8_t & alpha)
    {
        readPixel24(buffer, red, green, blue);
        alpha = *buffer++;
    }

    void Tga::writePixel24(uint8_t*& buffer, uint8_t red, uint8_t green, uint8_t blue)
    {
        *buffer++ = red;
        *buffer++ = green;
        *buffer++ = blue;
    }

    void Tga::writePixel32(uint8_t*& buffer, uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha)
    {
        writePixel24(buffer, red, green, blue);
        *buffer++ = alpha;
    }
}


