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

#include "KTXTextureFormat.h"
#include <cstring>

//
// https://www.khronos.org/opengles/sdk/tools/KTX/file_format_spec/
//
struct KTXHeader
{
    uint8_t  identifier[12];
    uint32_t endianness;
    uint32_t glType;
    uint32_t glTypeSize;
    uint32_t glFormat;
    uint32_t glInternalFormat;
    uint32_t glBaseInternalFormat;
    uint32_t pixelWidth;
    uint32_t pixelHeight;
    uint32_t pixelDepth;
    uint32_t numberOfArrayElements;
    uint32_t numberOfFaces;
    uint32_t numberOfMipmapLevels;
    uint32_t bytesOfKeyValueData;
};

static bool IsKTXFile(const void* data_, uint32_t size_)
{
    static const uint8_t KtxMagicTag[] = { 0xAB, 'K', 'T', 'X', ' ', '1', '1', 0xBB, '\r', '\n', '\x1A', '\n' };
    if (size_ >= sizeof(KtxMagicTag))
    {
        return (0 == memcmp(KtxMagicTag, data_, sizeof(KtxMagicTag)));
    }

    return false;
}

static const KTXHeader GetKtxHeader(const void* dataIn_, uint32_t sizeIn_)
{
    KTXHeader header;

    if (sizeIn_ > sizeof(KTXHeader))
    {
        memcpy(&header, dataIn_, sizeof(KTXHeader));
    }
    else
    {
        memset(&header, 0, sizeof(KTXHeader));
    }

    return header;
}

uint32_t ReadSizeAndIncrement(const uint8_t*& data_)
{
    uint32_t result = 0;
    memcpy(&result, data_, 4); // Using memcpy because data_ might not be double word aligned
    data_ += sizeof(uint32_t);

    return result;
}

static void LoadCompressedTexture(const void* dataIn_, gtf::scdr::TextureMemoryResource& textureOut_, uint32_t keyValueDataSize_)
{
    const gtf::scdr::Texture2DDescriptor& desc              = textureOut_.desc2d;
    const uint32_t                        offsetToImageData = sizeof(KTXHeader) + keyValueDataSize_;
    const uint8_t*                        imageData         = static_cast<const uint8_t*>(dataIn_) + offsetToImageData;

    for (uint32_t a = 0; a < desc.arraySize; ++a)
    {
        for (uint32_t i = 0; i < desc.mipLevels; i++)
        {
            const uint32_t pitch = ReadSizeAndIncrement(imageData);

            const uint32_t idx                = i + a * desc.mipLevels;
            textureOut_.data[idx].rowStride   = 0;
            textureOut_.data[idx].sliceStride = pitch;
            textureOut_.data[idx].data        = imageData;

            // Set the pointer of the source and destination data to the new location, respect padding
            const uint32_t pitchWithPadding = (pitch + (3 - ((pitch + 3) % 4)));
            imageData += pitchWithPadding;
        }
    }
}

static uint32_t BytesPerPixel(gtf::scdr::TextureFormat::Value format_)
{
    switch (format_)
    {
    case gtf::scdr::TextureFormat::R8G8B8:
        return 3;
    case gtf::scdr::TextureFormat::R8G8B8A8:
    case gtf::scdr::TextureFormat::R8G8B8A8_SRGB:
        return 4;
    default:
        return 0;
    }
}

static void LoadTexture2D(const void* dataIn_, gtf::scdr::TextureMemoryResource& textureOut_, uint32_t keyValueDataSize_)
{
    const gtf::scdr::Texture2DDescriptor& desc              = textureOut_.desc2d;
    const uint32_t                        offsetToImageData = sizeof(KTXHeader) + keyValueDataSize_;
    const uint8_t*                        imageData         = static_cast<const uint8_t*>(dataIn_) + offsetToImageData;
    const uint32_t                        pixelSizeInByte   = BytesPerPixel(desc.format);

    for (uint32_t a = 0; a < desc.arraySize; ++a)
    {
        uint32_t width  = desc.width;
        uint32_t height = desc.height;

        for (uint32_t i = 0; i < desc.mipLevels; i++)
        {
            imageData += sizeof(uint32_t); // Skip the image size

            const uint32_t pitch = width * height * pixelSizeInByte;

            const uint32_t rowStridePlusPad = (pixelSizeInByte * width + 3) & ~3;

            const uint32_t idx                = i + a * desc.mipLevels;
            textureOut_.data[idx].rowStride   = rowStridePlusPad;
            textureOut_.data[idx].sliceStride = pitch;
            textureOut_.data[idx].data        = imageData;

            imageData += height * rowStridePlusPad;

            width  = MAX(1, width >> 1);
            height = MAX(1, height >> 1);
        }
    }
}

static bool IsSupportedCompressedTextureFormat(gtf::scdr::TextureFormat::Value format_)
{
    switch (format_)
    {
    case gtf::scdr::TextureFormat::DXT1_RGB:
    case gtf::scdr::TextureFormat::DXT1_RGBA:
    case gtf::scdr::TextureFormat::DXT3_RGBA:
    case gtf::scdr::TextureFormat::DXT5_RGBA:
    case gtf::scdr::TextureFormat::ETC1_RGB:
    case gtf::scdr::TextureFormat::ETC2_RGB:
    case gtf::scdr::TextureFormat::ETC2_SRGB:
    case gtf::scdr::TextureFormat::ETC2_RGBA:
    case gtf::scdr::TextureFormat::ETC2_SRGBA:
    case gtf::scdr::TextureFormat::ETC2_RGBA_PUNCHTHROUGH:
    case gtf::scdr::TextureFormat::ETC2_SRGBA_PUNCHTHROUGH:
    case gtf::scdr::TextureFormat::EAC_R11:
    case gtf::scdr::TextureFormat::EAC_R11_SIGNED:
    case gtf::scdr::TextureFormat::EAC_RG11:
    case gtf::scdr::TextureFormat::EAC_RG11_SIGNED:
    case gtf::scdr::TextureFormat::PVRTC_RGB_2BPPV1:
    case gtf::scdr::TextureFormat::PVRTC_RGB_4BPPV1:
    case gtf::scdr::TextureFormat::PVRTC_RGBA_2BPPV1:
    case gtf::scdr::TextureFormat::PVRTC_RGBA_4BPPV1:
    case gtf::scdr::TextureFormat::PVRTC_RGBA_2BPPV2:
    case gtf::scdr::TextureFormat::PVRTC_RGBA_4BPPV2:
    case gtf::scdr::TextureFormat::ASTC_RGBA_4x4:
    case gtf::scdr::TextureFormat::ASTC_RGBA_5x4:
    case gtf::scdr::TextureFormat::ASTC_RGBA_5x5:
    case gtf::scdr::TextureFormat::ASTC_RGBA_6x5:
    case gtf::scdr::TextureFormat::ASTC_RGBA_6x6:
    case gtf::scdr::TextureFormat::ASTC_RGBA_8x5:
    case gtf::scdr::TextureFormat::ASTC_RGBA_8x6:
    case gtf::scdr::TextureFormat::ASTC_RGBA_8x8:
    case gtf::scdr::TextureFormat::ASTC_RGBA_10x5:
    case gtf::scdr::TextureFormat::ASTC_RGBA_10x6:
    case gtf::scdr::TextureFormat::ASTC_RGBA_10x8:
    case gtf::scdr::TextureFormat::ASTC_RGBA_10x10:
    case gtf::scdr::TextureFormat::ASTC_RGBA_12x10:
    case gtf::scdr::TextureFormat::ASTC_RGBA_12x12:
    case gtf::scdr::TextureFormat::ASTC_SRGBA_4x4:
    case gtf::scdr::TextureFormat::ASTC_SRGBA_5x4:
    case gtf::scdr::TextureFormat::ASTC_SRGBA_5x5:
    case gtf::scdr::TextureFormat::ASTC_SRGBA_6x5:
    case gtf::scdr::TextureFormat::ASTC_SRGBA_6x6:
    case gtf::scdr::TextureFormat::ASTC_SRGBA_8x5:
    case gtf::scdr::TextureFormat::ASTC_SRGBA_8x6:
    case gtf::scdr::TextureFormat::ASTC_SRGBA_8x8:
    case gtf::scdr::TextureFormat::ASTC_SRGBA_10x5:
    case gtf::scdr::TextureFormat::ASTC_SRGBA_10x6:
    case gtf::scdr::TextureFormat::ASTC_SRGBA_10x8:
    case gtf::scdr::TextureFormat::ASTC_SRGBA_10x10:
    case gtf::scdr::TextureFormat::ASTC_SRGBA_12x10:
    case gtf::scdr::TextureFormat::ASTC_SRGBA_12x12:
        return true;

    default:
        return false;
    }
}

static bool IsSupportedUnCompressedTextureFormat(gtf::scdr::TextureFormat::Value format_)
{
    return (format_ == gtf::scdr::TextureFormat::R8G8B8A8) || (format_ == gtf::scdr::TextureFormat::R8G8B8A8_SRGB);
}

static gtf::scdr::TextureFormat::Value textureFormatKTX2SCDR(uint32_t format_)
{
    switch (format_)
    {
    case KTXTextureFormat::GL_ETC1_RGB8_OES:
        return gtf::scdr::TextureFormat::ETC1_RGB;
    case KTXTextureFormat::GL_COMPRESSED_RGB8_ETC2:
        return gtf::scdr::TextureFormat::ETC2_RGB;
    case KTXTextureFormat::GL_COMPRESSED_SRGB8_ETC2:
        return gtf::scdr::TextureFormat::ETC2_SRGB;
    case KTXTextureFormat::GL_COMPRESSED_RGBA8_ETC2_EAC:
        return gtf::scdr::TextureFormat::ETC2_RGBA;
    case KTXTextureFormat::GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC:
        return gtf::scdr::TextureFormat::ETC2_SRGBA;
    case KTXTextureFormat::GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2:
        return gtf::scdr::TextureFormat::ETC2_RGBA_PUNCHTHROUGH;
    case KTXTextureFormat::GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2:
        return gtf::scdr::TextureFormat::ETC2_SRGBA_PUNCHTHROUGH;
    case KTXTextureFormat::GL_COMPRESSED_R11_EAC:
        return gtf::scdr::TextureFormat::EAC_R11;
    case KTXTextureFormat::GL_COMPRESSED_SIGNED_R11_EAC:
        return gtf::scdr::TextureFormat::EAC_R11_SIGNED;
    case KTXTextureFormat::GL_COMPRESSED_RG11_EAC:
        return gtf::scdr::TextureFormat::EAC_RG11;
    case KTXTextureFormat::GL_COMPRESSED_SIGNED_RG11_EAC:
        return gtf::scdr::TextureFormat::EAC_RG11_SIGNED;
    case KTXTextureFormat::GL_RGBA8:
    case KTXTextureFormat::GL_RGBA8UI:
        return gtf::scdr::TextureFormat::R8G8B8A8;
    case KTXTextureFormat::GL_SRGB8_ALPHA8:
        return gtf::scdr::TextureFormat::R8G8B8A8_SRGB;
    case KTXTextureFormat::GL_RGB8:
        return gtf::scdr::TextureFormat::R8G8B8;
    case KTXTextureFormat::GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG:
        return gtf::scdr::TextureFormat::PVRTC_RGB_2BPPV1;
    case KTXTextureFormat::GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG:
        return gtf::scdr::TextureFormat::PVRTC_RGB_4BPPV1;
    case KTXTextureFormat::GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG:
        return gtf::scdr::TextureFormat::PVRTC_RGBA_2BPPV1;
    case KTXTextureFormat::GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG:
        return gtf::scdr::TextureFormat::PVRTC_RGBA_4BPPV1;
    case KTXTextureFormat::GL_COMPRESSED_RGBA_PVRTC_2BPPV2_IMG:
        return gtf::scdr::TextureFormat::PVRTC_RGBA_2BPPV2;
    case KTXTextureFormat::GL_COMPRESSED_RGBA_PVRTC_4BPPV2_IMG:
        return gtf::scdr::TextureFormat::PVRTC_RGBA_4BPPV2;
    case KTXTextureFormat::GL_COMPRESSED_RGBA_ASTC_4x4_KHR:
        return gtf::scdr::TextureFormat::ASTC_RGBA_4x4;
    case KTXTextureFormat::GL_COMPRESSED_RGBA_ASTC_5x4_KHR:
        return gtf::scdr::TextureFormat::ASTC_RGBA_5x4;
    case KTXTextureFormat::GL_COMPRESSED_RGBA_ASTC_5x5_KHR:
        return gtf::scdr::TextureFormat::ASTC_RGBA_5x5;
    case KTXTextureFormat::GL_COMPRESSED_RGBA_ASTC_6x5_KHR:
        return gtf::scdr::TextureFormat::ASTC_RGBA_6x5;
    case KTXTextureFormat::GL_COMPRESSED_RGBA_ASTC_6x6_KHR:
        return gtf::scdr::TextureFormat::ASTC_RGBA_6x6;
    case KTXTextureFormat::GL_COMPRESSED_RGBA_ASTC_8x5_KHR:
        return gtf::scdr::TextureFormat::ASTC_RGBA_8x5;
    case KTXTextureFormat::GL_COMPRESSED_RGBA_ASTC_8x6_KHR:
        return gtf::scdr::TextureFormat::ASTC_RGBA_8x6;
    case KTXTextureFormat::GL_COMPRESSED_RGBA_ASTC_8x8_KHR:
        return gtf::scdr::TextureFormat::ASTC_RGBA_8x8;
    case KTXTextureFormat::GL_COMPRESSED_RGBA_ASTC_10x5_KHR:
        return gtf::scdr::TextureFormat::ASTC_RGBA_10x5;
    case KTXTextureFormat::GL_COMPRESSED_RGBA_ASTC_10x6_KHR:
        return gtf::scdr::TextureFormat::ASTC_RGBA_10x6;
    case KTXTextureFormat::GL_COMPRESSED_RGBA_ASTC_10x8_KHR:
        return gtf::scdr::TextureFormat::ASTC_RGBA_10x8;
    case KTXTextureFormat::GL_COMPRESSED_RGBA_ASTC_10x10_KHR:
        return gtf::scdr::TextureFormat::ASTC_RGBA_10x10;
    case KTXTextureFormat::GL_COMPRESSED_RGBA_ASTC_12x10_KHR:
        return gtf::scdr::TextureFormat::ASTC_RGBA_12x10;
    case KTXTextureFormat::GL_COMPRESSED_RGBA_ASTC_12x12_KHR:
        return gtf::scdr::TextureFormat::ASTC_RGBA_12x12;
    case KTXTextureFormat::GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR:
        return gtf::scdr::TextureFormat::ASTC_SRGBA_4x4;
    case KTXTextureFormat::GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x4_KHR:
        return gtf::scdr::TextureFormat::ASTC_SRGBA_5x4;
    case KTXTextureFormat::GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR:
        return gtf::scdr::TextureFormat::ASTC_SRGBA_5x5;
    case KTXTextureFormat::GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x5_KHR:
        return gtf::scdr::TextureFormat::ASTC_SRGBA_6x5;
    case KTXTextureFormat::GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR:
        return gtf::scdr::TextureFormat::ASTC_SRGBA_6x6;
    case KTXTextureFormat::GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x5_KHR:
        return gtf::scdr::TextureFormat::ASTC_SRGBA_8x5;
    case KTXTextureFormat::GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x6_KHR:
        return gtf::scdr::TextureFormat::ASTC_SRGBA_8x6;
    case KTXTextureFormat::GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR:
        return gtf::scdr::TextureFormat::ASTC_SRGBA_8x8;
    case KTXTextureFormat::GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x5_KHR:
        return gtf::scdr::TextureFormat::ASTC_SRGBA_10x5;
    case KTXTextureFormat::GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR:
        return gtf::scdr::TextureFormat::ASTC_SRGBA_10x6;
    case KTXTextureFormat::GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x8_KHR:
        return gtf::scdr::TextureFormat::ASTC_SRGBA_10x8;
    case KTXTextureFormat::GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR:
        return gtf::scdr::TextureFormat::ASTC_SRGBA_10x10;
    case KTXTextureFormat::GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x10_KHR:
        return gtf::scdr::TextureFormat::ASTC_SRGBA_12x10;
    case KTXTextureFormat::GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR:
        return gtf::scdr::TextureFormat::ASTC_SRGBA_12x12;
    case KTXTextureFormat::GL_COMPRESSED_RGB_S3TC_DXT1_EXT:
        return gtf::scdr::TextureFormat::DXT1_RGB;
    case KTXTextureFormat::GL_COMPRESSED_RGBA_S3TC_DXT1_EXT:
        return gtf::scdr::TextureFormat::DXT1_RGBA;
    case KTXTextureFormat::GL_COMPRESSED_RGBA_S3TC_DXT3_EXT:
        return gtf::scdr::TextureFormat::DXT3_RGBA;
    case KTXTextureFormat::GL_COMPRESSED_RGBA_S3TC_DXT5_EXT:
        return gtf::scdr::TextureFormat::DXT5_RGBA;
    default:
        return gtf::scdr::TextureFormat::Invalid;
    }
}

class KTXTextureDecoder : public gtf::resourcesystem::Decoder
{
public:
    KTXTextureDecoder(const gtf::resourcesystem::FactoryHandle& resourceFactory_)
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

        gtf::scdr::decoder::Texture        outputData{ result };
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
        if (!IsKTXFile(dataIn_, sizeIn_))
        {
            return false;
        }

        const KTXHeader header = GetKtxHeader(dataIn_, sizeIn_);

        textureOut_.dimension        = gtf::scdr::TextureMemoryResource::Texture2D;
        textureOut_.desc2d.arraySize = 1;
        textureOut_.desc2d.flags     = 0;
        textureOut_.desc2d.format    = textureFormatKTX2SCDR(header.glInternalFormat);
        textureOut_.desc2d.height    = header.pixelHeight;
        textureOut_.desc2d.mipLevels = header.numberOfMipmapLevels;
        textureOut_.desc2d.width     = header.pixelWidth;
        textureOut_.data             = nullptr;

        const uint32_t keyValueDataSize = header.bytesOfKeyValueData;

        const uint32_t textureDataArraySize = textureOut_.desc2d.mipLevels * textureOut_.desc2d.arraySize * sizeof(gtf::scdr::TextureData);
        textureDataArrayBufferOut_          = gtf::container::SharedBuffer::create(textureDataArraySize);

        if (!textureDataArrayBufferOut_ || textureDataArrayBufferOut_->getData() == nullptr)
        {
            return false;
        }

        textureOut_.data = static_cast<gtf::scdr::TextureData*>(textureDataArrayBufferOut_->getData());

        if (IsSupportedCompressedTextureFormat(textureOut_.desc2d.format))
        {
            LoadCompressedTexture(dataIn_, textureOut_, keyValueDataSize);
            return true;
        }
        else if (IsSupportedUnCompressedTextureFormat(textureOut_.desc2d.format))
        {
            LoadTexture2D(dataIn_, textureOut_, keyValueDataSize);
            return true;
        }

        return false;
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
        gtf::resourcesystem::DecoderHandle ktxDecoder = new KTXTextureDecoder(factory_);

        if (ktxDecoder.valid())
        {
            decoders.append(ktxDecoder);
            manager->addDefaultDecodingStep(ktxDecoder->getInputFormat(), ktxDecoder->getOutputFormat());
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
