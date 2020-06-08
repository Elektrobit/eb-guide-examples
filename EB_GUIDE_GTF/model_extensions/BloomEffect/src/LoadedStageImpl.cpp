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

#include "PreparedStageImpl.h"
#include "LoadedStageImpl.h"
#include "BloomEffectRenderer.h"

bloom::LoadedStageImpl::LoadedStageImpl(const gtf::dependencyresolver::DependencyContainerHandle& container_)
    : container(container_)
{
    container->getAll<gtf::scdr::SDK>(sdkHandles);

    for (gtf::container::Array<gtf::scdr::SDKHandle>::Iterator it = sdkHandles.begin(), e = sdkHandles.end(); it != e; ++it)
    {
        // Register the custom renderer at the SCDR
        registerRenderer(*it);
    }
}

bloom::LoadedStageImpl::~LoadedStageImpl()
{
    for (gtf::container::Array<gtf::scdr::SDKHandle>::Iterator it = sdkHandles.begin(), e = sdkHandles.end(); it != e; ++it)
    {
        unregisterRenderer(*it);
    }
    sdkHandles.clear();
}

bool bloom::LoadedStageImpl::registerRenderer(gtf::scdr::SDKHandle& sdk_)
{
    if (sdk_->getGraphicsApi() == gtf::scdr::GraphicsApi::OpenGLES20)
    {
        return false;
    }

    gtf::scdr::ModuleDescriptor desc;
    desc.className       = BLOOMEFFECT_PLUGIN_NAME;
    desc.createCallback  = BloomEffectRenderer::createIntance;
    desc.destroyCallback = BloomEffectRenderer::destroyInstance;
    desc.createParams    = &sdk_;

    return sdk_->registerModule(desc);
}

void bloom::LoadedStageImpl::unregisterRenderer(gtf::scdr::SDKHandle& sdk_)
{
    sdk_->unregisterModule(BLOOMEFFECT_PLUGIN_NAME);
}
