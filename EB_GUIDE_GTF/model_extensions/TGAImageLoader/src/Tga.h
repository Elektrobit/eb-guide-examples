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
#ifndef TGA_IMAGE_LOADER_TGA_H
#define TGA_IMAGE_LOADER_TGA_H

#include <gtf/decoder/MemImage.h>
#include <gtf/scdr/resources/ResourceLoader.h>
#include <gtf/resourcesystem/Factory.h>

namespace tgaimageloader
{
    class Tga : public gtf::types::Noncopyable, public gtf::osal::memory::Memory
    {
    public:
        Tga();

        ~Tga();

        bool getInfo(const void* srcData, uint32_t srcSize, gtf::decoder::MemImage::Info& info);
        void flipImage(gtf::decoder::MemImage::Info& info);

        bool getData(const void* srcData, uint32_t srcSize, gtf::decoder::MemImage::Info& info);

        unsigned char headerData[18];

    private:
        bool imageCompressed;

        bool isImageReversed;

        uint32_t bytesPerPixel;

        void decodeCompressed(uint8_t* inBuffer, uint8_t* outBuffer, gtf::decoder::MemImage::Info& info);
        void decodeUncompressed(uint8_t* inBuffer, uint8_t* outBuffer, gtf::decoder::MemImage::Info& info);

        void readPixel24(uint8_t*& buffer, uint8_t& red, uint8_t& green, uint8_t& blue);
        void readPixel32(uint8_t*& buffer, uint8_t& red, uint8_t& green, uint8_t& blue, uint8_t& alpha);
        void writePixel24(uint8_t*& buffer, uint8_t red, uint8_t green, uint8_t blue);
        void writePixel32(uint8_t*& buffer, uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha);

        void TransferPixel32(uint8_t *&inBuffer, uint8_t *&outBuffer);

        void TransferPixel24(uint8_t *&inBuffer, uint8_t *&outBuffer);

        void TransferPixels(uint8_t *&inBuffer, uint8_t *&outBuffer, size_t numberOfPixels);

        void writePixel32Multiple(uint8_t *&outBuffer, size_t numberOfTimes, uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha);
        void writePixel24Multiple(uint8_t *&outBuffer, size_t numberOfTimes, uint8_t red, uint8_t green, uint8_t blue);
    };
}
#endif //TGA_IMAGE_LOADER_TGA_H