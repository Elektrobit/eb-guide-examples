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
#include <gtf/dependencyresolver/DependencyContainer.h>
#include <gtf/datapool/Datapool.h>
#include <gtf/decoder/EncodedMemImage.h>
#include "InitializedStageImpl.h"
#include "DecoderImpl.h"
#include <gtf/decoder/MemImage.h>

GTF_PLUGIN_SO_SYMBOL const char* GTF_PLUGIN_GET_VERSION()
{
    return GTF_PLUGIN_VERSION_STRING;
}

static gtf::resourcesystem::DecoderHandle createDecoder(const gtf::dependencyresolver::DependencyContainerHandle& container, const char* inputFormatName, const char* outputFormatName)
{
    if (!container)
    {
        return gtf::resourcesystem::DecoderHandle();
    }

    const gtf::resourcesystem::FactoryHandle factory = container->get<gtf::resourcesystem::Factory>();
    if (!factory)
    {
        return gtf::resourcesystem::DecoderHandle();
    }

    const gtf::resourcesystem::FormatHandle inputFormat = factory->createFormat(inputFormatName);
    if (!inputFormat)
    {
        return gtf::resourcesystem::DecoderHandle();
    }

    const gtf::resourcesystem::FormatHandle outputFormat = factory->createFormat(outputFormatName);
    if (!outputFormat)
    {
        return gtf::resourcesystem::DecoderHandle();
    }

    return new tgaimageloader::DecoderImpl(inputFormat, outputFormat, factory);
}

static gtf::dependencyresolver::InterfaceHandle createInitializedStageImpl(const gtf::dependencyresolver::DependencyContainerHandle& container)
{
    if (!container)
    {
        return gtf::dependencyresolver::InterfaceHandle();
    }

    const gtf::resourcesystem::DecodingManagerHandle decodingManagerHandle = container->get<gtf::resourcesystem::DecodingManager>();

    if (!decodingManagerHandle)
    {
        return gtf::dependencyresolver::InterfaceHandle();
    }

    gtf::container::Array<gtf::resourcesystem::DecoderHandle> decoders;

    gtf::resourcesystem::DecoderHandle decoder = createDecoder(container, gtf::decoder::EncodedMemImage::getFormatName(), gtf::decoder::MemImage::getFormatName());
    decoders.append(decoder);

    return new tgaimageloader::InitializedStageImpl(decoders, decodingManagerHandle);
}

GTF_PLUGIN_SO_SYMBOL void GTF_PLUGIN_EXTEND_CATALOG(void* catalog)
{
    if (NULL == catalog)
    {
        return;
    }

    gtf::dependencyresolver::DependencyCatalog& dependencyCatalog = *static_cast<gtf::dependencyresolver::DependencyCatalog*>(catalog);
    gtf::dependencyresolver::extendCatalog<gtf::model::InitializedStage, createInitializedStageImpl >(dependencyCatalog);
}
