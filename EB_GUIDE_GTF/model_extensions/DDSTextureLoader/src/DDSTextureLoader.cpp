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

#include <gtf/pluginloader/PluginSymbols.h>
#include <gtf/pluginloader/PluginData.h>
#include <gtf/model/InitializedStage.h>
#include <gtf/resourcesystem/Factory.h>
#include <gtf/resourcesystem/Decoder.h>
#include <gtf/resourcesystem/DecodingManager.h>
#include <gtf/decoder/EncodedMemImage.h>
#include <gtf/scdr/gpu/TextureData.h>
#include <gtf/scdr/gpu/TextureFormat.h>
#include <gtf/scdr/resources/TextureMemoryResource.h>
#include <gtf/scdr/resources/TextureResource.h>
#include <gtf/tracing/Logging.h>
#include <gtf/container/SharedBuffer.h>

#include <cstring>

//
// https://msdn.microsoft.com/en-us/library/windows/desktop/bb943982(v=vs.85).aspx
//
struct DDS_FLAGS
{
    enum Value
    {
        DDSD_CAPS        = 0x1,
        DDSD_HEIGHT      = 0x2,
        DDSD_WIDTH       = 0x4,
        DDSD_PITCH       = 0x8,
        DDSD_PIXELFORMAT = 0x1000,
        DDSD_MIPMAPCOUNT = 0x20000,
        DDSD_LINEARSIZE  = 0x80000,
        DDSD_DEPTH       = 0x800000,
    };
};

//
// https://msdn.microsoft.com/en-us/library/windows/desktop/bb943984(v=vs.85).aspx
//
struct DDPF_FLAGS
{
    enum Value
    {
        DDPF_ALPHAPIXELS = 0x1,
        DDPF_ALPHA       = 0x2,
        DDPF_FOURCC      = 0x4,
        DDPF_RGB         = 0x40,
        DDPF_YUV         = 0x200,
        DDPF_LUMINANCE   = 0x20000,
        DDPF_RGBA        = DDPF_RGB | DDPF_ALPHA
    };
};

//
// https://msdn.microsoft.com/en-us/library/windows/desktop/bb943984(v=vs.85).aspx
//
struct DDS_PIXELFORMAT
{
    uint32_t dwSize;
    uint32_t dwFlags;
    uint32_t dwFourCC;
    uint32_t dwRGBBitCount;
    uint32_t dwRBitMask;
    uint32_t dwGBitMask;
    uint32_t dwBBitMask;
    uint32_t dwABitMask;
};

//
// https://msdn.microsoft.com/en-us/library/windows/desktop/bb943982(v=vs.85).aspx
//
struct DDS_HEADER
{
    uint32_t        dwSize;
    uint32_t        dwFlags;
    uint32_t        dwHeight;
    uint32_t        dwWidth;
    uint32_t        dwPitchOrLinearSize;
    uint32_t        dwDepth;
    uint32_t        dwMipMapCount;
    uint32_t        dwReserved1[11];
    DDS_PIXELFORMAT ddspf;
    uint32_t        dwCaps;
    uint32_t        dwCaps2;
    uint32_t        dwCaps3;
    uint32_t        dwCaps4;
    uint32_t        dwReserved2;
};

//
// https://msdn.microsoft.com/en-us/library/windows/desktop/bb943991(v=vs.85).aspx
//
uint32_t GetBlockCompressionStride(uint32_t width_, uint32_t blockSize_)
{
    return MAX(1, ((width_ + 3) / 4)) * blockSize_;
}

uint32_t GetBlockCompressionPitch(uint32_t width_, uint32_t height_, uint32_t blockSize_)
{
    return MAX(1, ((width_ + 3) / 4)) * MAX(1, ((height_ + 3) / 4)) * blockSize_;
}

//
// https://msdn.microsoft.com/en-us/library/windows/desktop/bb943991(v=vs.85).aspx
//
static bool IsDDSFile(const void* dataIn_, uint32_t sizeIn_)
{
    const uint32_t magic = 0x20534444;

    if (sizeIn_ >= sizeof(magic))
    {
        return memcmp(dataIn_, &magic, sizeof(magic)) == 0;
    }
    else
    {
        return false;
    }
}

static DDS_HEADER GetDDSHeader(const void* dataIn_)
{
    DDS_HEADER     ddsHeader;
    const uint8_t* data = static_cast<const uint8_t*>(dataIn_) + sizeof(uint32_t);
    memcpy(&ddsHeader, data, sizeof(ddsHeader));

    return ddsHeader;
}

static bool IsDXT1(const DDS_HEADER& header_)
{
    return memcmp(&header_.ddspf.dwFourCC, "DXT1", 4) == 0;
}

static bool IsDXT3(const DDS_HEADER& header_)
{
    return memcmp(&header_.ddspf.dwFourCC, "DXT3", 4) == 0;
}

static bool IsDXT5(const DDS_HEADER& header_)
{
    return memcmp(&header_.ddspf.dwFourCC, "DXT5", 4) == 0;
}

static bool IsDXTCompressed(const DDS_HEADER& header_)
{
    return IsDXT1(header_) || IsDXT3(header_) || IsDXT5(header_);
}

static bool IsValid(const DDS_HEADER& header_)
{
    if (header_.dwSize != 124)
    {
        return false;
    }

    if (!(header_.dwFlags & DDS_FLAGS::DDSD_HEIGHT) && (header_.dwFlags & DDS_FLAGS::DDSD_WIDTH) && (header_.dwFlags & DDS_FLAGS::DDSD_PIXELFORMAT))
    {
        return false;
    }

    if (header_.ddspf.dwSize != 32)
    {
        return false;
    }

    if (!((header_.dwFlags & DDPF_FLAGS::DDPF_FOURCC) | (header_.dwFlags & DDPF_FLAGS::DDPF_RGBA)))
    {
        // we support only uncompressed RGBA and compressed formats
        return false;
    }

    if ((header_.dwFlags & DDPF_FLAGS::DDPF_FOURCC))
    {
        if (!IsDXTCompressed(header_))
        {
            // we support only DXT1, DXT3, and DXT5 compression
            return false;
        }
    }

    return true;
}

static gtf::scdr::TextureFormat::Value GetTextureFormat(const DDS_HEADER& header_)
{
    if (IsDXT1(header_))
    {
        if(header_.ddspf.dwFlags | DDPF_FLAGS::DDPF_ALPHAPIXELS)
        {
            return gtf::scdr::TextureFormat::DXT1_RGBA;
        }
        else
        {
            return gtf::scdr::TextureFormat::DXT1_RGB;
        }
    }
    else if (IsDXT3(header_))
    {
        return gtf::scdr::TextureFormat::DXT3_RGBA;
    }
    else if (IsDXT5(header_))
    {
        return gtf::scdr::TextureFormat::DXT5_RGBA;
    }
    else if (header_.ddspf.dwFlags & DDPF_FLAGS::DDPF_RGBA)
    {
        const uint32_t channelMask = header_.ddspf.dwABitMask
                                     | header_.ddspf.dwBBitMask
                                     | header_.ddspf.dwGBitMask
                                     | header_.ddspf.dwRBitMask;

        if (channelMask == 0xFFFFFFFF)
        {
            return gtf::scdr::TextureFormat::R8G8B8A8;
        }
    }

    return gtf::scdr::TextureFormat::Unknown;
}

static void LoadCompressedTexture(const void* dataIn_, gtf::scdr::TextureMemoryResource& textureOut_, uint32_t blockSize_)
{
    const uint8_t* data = static_cast<const uint8_t*>(dataIn_) + sizeof(DDS_HEADER) + sizeof(uint32_t);

    const gtf::scdr::Texture2DDescriptor desc = textureOut_.desc2d;

    for (uint32_t a = 0; a < desc.arraySize; ++a)
    {
        uint32_t width  = desc.width;
        uint32_t height = desc.height;

        for (uint32_t m = 0; m < desc.mipLevels; ++m)
        {
            const uint32_t idx = m + a * desc.mipLevels;

            const uint32_t stride = GetBlockCompressionStride(width, blockSize_);
            const uint32_t pitch  = GetBlockCompressionPitch(width, height, blockSize_);

            textureOut_.data[idx].rowStride   = stride;
            textureOut_.data[idx].sliceStride = pitch;
            textureOut_.data[idx].data        = data;

            width  = MAX(1, width >> 1);
            height = MAX(1, height >> 1);
            data += pitch;
        }
    }
}

static void LoadTexture2D(const void* dataIn_, gtf::scdr::TextureMemoryResource& textureOut_, uint32_t bitsPerPixel_)
{
    const uint8_t* data = static_cast<const uint8_t*>(dataIn_) + sizeof(DDS_HEADER) + sizeof(uint32_t);

    const gtf::scdr::Texture2DDescriptor desc = textureOut_.desc2d;

    for (uint32_t a = 0; a < desc.arraySize; ++a)
    {
        uint32_t width  = desc.width;
        uint32_t height = desc.height;

        for (uint32_t m = 0; m < desc.mipLevels; ++m)
        {
            const uint32_t idx = m + a * desc.mipLevels;

            //
            // https://msdn.microsoft.com/de-de/library/windows/desktop/bb943991(v=vs.85).aspx
            //
            const uint32_t stride = (width * bitsPerPixel_ + 7) / 8;
            const uint32_t pitch  = stride * height;

            textureOut_.data[idx].rowStride   = stride;
            textureOut_.data[idx].sliceStride = pitch;
            textureOut_.data[idx].data        = data;

            width  = MAX(1, width >> 1);
            height = MAX(1, height >> 1);
            data += pitch;
        }
    }
}

class DDSTextureDecoder : public gtf::resourcesystem::Decoder
{
public:
    DDSTextureDecoder(const gtf::resourcesystem::FactoryHandle& resourceFactory_)
        : resourceFactory(resourceFactory_)
        , inputFormat(resourceFactory_->createFormat(gtf::decoder::EncodedMemImage::getFormatName()))
        , outputFormat(resourceFactory_->createFormat(gtf::scdr::decoder::Texture::getFormatName()))
    {
    }

    virtual gtf::resourcesystem::FormatHandle getInputFormat() const
    {
        return inputFormat;
    }

    virtual gtf::resourcesystem::FormatHandle getOutputFormat() const
    {
        return outputFormat;
    }

    virtual gtf::resourcesystem::DataHandle decode(const gtf::resourcesystem::ResourceHandle& resource, const gtf::resourcesystem::DataHandle& data) const
    {
        if (!data)
        {
            return gtf::resourcesystem::DataHandle();
        }

        gtf::decoder::EncodedMemImage               inputData(data);
        const char*                                 inputPath   = inputData.getPath();
        gtf::decoder::EncodedMemImage::BufferHandle inputBuffer = inputData.getData();
        if (!inputPath || !inputBuffer)
        {
            return gtf::resourcesystem::DataHandle();
        }

        gtf::resourcesystem::DataHandle result = resourceFactory->createData(outputFormat);
        if (!result)
        {
            return gtf::resourcesystem::DataHandle();
        }

        gtf::scdr::decoder::Texture        outputData {result};
        gtf::scdr::TextureMemoryResource   textureMemoryResource;
        gtf::container::SharedBufferHandle textureDataArrayBuffer;

        if (!decode(inputBuffer->getData(), inputBuffer->getSize(), textureMemoryResource, textureDataArrayBuffer))
        {
            return gtf::resourcesystem::DataHandle();
        }

        if (!outputData.setTextureData(textureMemoryResource, textureDataArrayBuffer, inputBuffer))
        {
            return gtf::resourcesystem::DataHandle();
        }

        resource->attachData(result);
        resource->detachData(data);
        return result;
    }

    static bool decode(const void* dataIn_, uint32_t sizeIn_,
        gtf::scdr::TextureMemoryResource& textureOut_, gtf::container::SharedBufferHandle& textureDataArrayBufferOut_)
    {
        if (!IsDDSFile(dataIn_, sizeIn_))
        {
            return false;
        }

        const DDS_HEADER header = GetDDSHeader(dataIn_);
        if (!IsValid(header))
        {
            return false;
        }

        textureOut_.dimension        = gtf::scdr::TextureMemoryResource::Texture2D;
        textureOut_.desc2d.arraySize = 1;
        textureOut_.desc2d.flags     = 0;
        textureOut_.desc2d.format    = GetTextureFormat(header);
        textureOut_.desc2d.height    = header.dwHeight;
        textureOut_.desc2d.mipLevels = header.dwMipMapCount;
        textureOut_.desc2d.width     = header.dwWidth;
        textureOut_.data             = nullptr;

        const uint32_t textureDataArraySize = textureOut_.desc2d.mipLevels * textureOut_.desc2d.arraySize * sizeof(gtf::scdr::TextureData);
        textureDataArrayBufferOut_          = gtf::container::SharedBuffer::create(textureDataArraySize);

        if (!textureDataArrayBufferOut_ || textureDataArrayBufferOut_->getData() == nullptr)
        {
            return false;
        }

        textureOut_.data = static_cast<gtf::scdr::TextureData*>(textureDataArrayBufferOut_->getData());

        switch (textureOut_.desc2d.format)
        {
        case gtf::scdr::TextureFormat::R8G8B8A8:
            LoadTexture2D(dataIn_, textureOut_, 32);
            break;

        case gtf::scdr::TextureFormat::DXT1_RGB:
        case gtf::scdr::TextureFormat::DXT1_RGBA:
            LoadCompressedTexture(dataIn_, textureOut_, 8);
            break;

        case gtf::scdr::TextureFormat::DXT3_RGBA:
        case gtf::scdr::TextureFormat::DXT5_RGBA:
            LoadCompressedTexture(dataIn_, textureOut_, 16);
            break;

        default:
            return false;
        }

        return true;
    }

    gtf::resourcesystem::FactoryHandle resourceFactory;
    gtf::resourcesystem::FormatHandle  inputFormat;
    gtf::resourcesystem::FormatHandle  outputFormat;
};

class InitializedStageImpl : public gtf::model::InitializedStage, public gtf::types::Noncopyable
{
public:
    InitializedStageImpl(const gtf::resourcesystem::FactoryHandle& factory_, const gtf::resourcesystem::DecodingManagerHandle& manager_)
        : manager(manager_)
    {
        gtf::resourcesystem::DecoderHandle ddsDecoder = new DDSTextureDecoder(factory_);

        if (ddsDecoder.valid())
        {
            decoders.append(ddsDecoder);
            manager->addDefaultDecodingStep(ddsDecoder->getInputFormat(), ddsDecoder->getOutputFormat());
        }

        manager->registerDecoders(decoders);
    }

    virtual ~InitializedStageImpl()
    {
        manager->unregisterDecoders(decoders);
    }

private:
    gtf::resourcesystem::DecodingManagerHandle                manager;
    gtf::container::Array<gtf::resourcesystem::DecoderHandle> decoders;
};

static gtf::dependencyresolver::InterfaceHandle createInitializedStageImpl(const gtf::dependencyresolver::DependencyContainerHandle& container)
{
    if (!container)
    {
        return gtf::dependencyresolver::InterfaceHandle();
    }

    const gtf::resourcesystem::DecodingManagerHandle manager = container->get<gtf::resourcesystem::DecodingManager>();
    if (!manager)
    {
        return gtf::dependencyresolver::InterfaceHandle();
    }

    const gtf::resourcesystem::FactoryHandle factory = container->get<gtf::resourcesystem::Factory>();
    if (!factory)
    {
        return gtf::dependencyresolver::InterfaceHandle();
    }

    return new InitializedStageImpl(factory, manager);
}

GTF_PLUGIN_SO_SYMBOL const char* GTF_PLUGIN_GET_VERSION()
{
    return GTF_PLUGIN_VERSION_STRING;
}

GTF_PLUGIN_SO_SYMBOL void GTF_PLUGIN_EXTEND_CATALOG(void* catalog)
{
    if (!catalog)
    {
        return;
    }

    gtf::dependencyresolver::DependencyCatalog& dependencyCatalog = *static_cast<gtf::dependencyresolver::DependencyCatalog*>(catalog);
    gtf::dependencyresolver::extendCatalog<gtf::model::InitializedStage, createInitializedStageImpl>(dependencyCatalog);
}
