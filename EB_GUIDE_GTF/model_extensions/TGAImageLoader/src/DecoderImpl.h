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
#ifndef TGA_IMAGE_LOADER_DECODERI_IMPL_H
#define TGA_IMAGE_LOADER_DECODERI_IMPL_H

#include <gtf/resourcesystem/Decoder.h>
#include <gtf/scdr/resources/ResourceLoader.h>
#include <gtf/resourcesystem/Factory.h>

namespace tgaimageloader
{
    class DecoderImpl : public gtf::resourcesystem::Decoder
    {
    public:
        DecoderImpl(const gtf::resourcesystem::FormatHandle& inputFormat_, const gtf::resourcesystem::FormatHandle& outputFormat_, const gtf::resourcesystem::FactoryHandle& factory_);

        virtual gtf::resourcesystem::DataHandle decode(const gtf::resourcesystem::ResourceHandle& resource, const gtf::resourcesystem::DataHandle& data) const;

        static bool matchStringEnd(const char* path, uint32_t pathLength, const char* match, uint32_t matchLength);

        virtual gtf::resourcesystem::FormatHandle getInputFormat() const;


        virtual gtf::resourcesystem::FormatHandle getOutputFormat() const;

    protected:

        gtf::resourcesystem::FormatHandle  inputFormat;
        gtf::resourcesystem::FormatHandle  outputFormat;
        gtf::resourcesystem::FactoryHandle factory;
    };
}// namespace tgaimageloader

#endif //TGA_IMAGE_LOADER_DECODERI_IMPL_H