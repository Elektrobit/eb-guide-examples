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
#include "LoadedStageImpl.h"
#include "Multisample.h"
#include <gtf/scdr/widgets/ModuleDescriptor.h>

multisample::LoadedStageImpl::LoadedStageImpl(const gtf::dependencyresolver::DependencyContainerHandle& container_)
    : container(container_)
{
    container->getAll<gtf::scdr::SDK>(sdkHandles);

    for (gtf::container::Array<gtf::scdr::SDKHandle>::Iterator it = sdkHandles.begin(), e = sdkHandles.end(); it != e; ++it)
    {
        registerRenderer(*it);
    }
}

multisample::LoadedStageImpl::~LoadedStageImpl()
{
    for (gtf::container::Array<gtf::scdr::SDKHandle>::Iterator it = sdkHandles.begin(), e = sdkHandles.end(); it != e; ++it)
    {
        unregisterRenderer(*it);
    }
    sdkHandles.clear();
}

bool multisample::LoadedStageImpl::registerRenderer(gtf::scdr::SDKHandle& sdk_)
{
    gtf::scdr::ModuleDescriptor desc;
    desc.className       = "Multisample";
    desc.createCallback  = Renderer::createInstance;
    desc.destroyCallback = Renderer::destroyInstance;
    desc.createParams    = &sdk_;

    return sdk_->registerModule(desc);
}

void multisample::LoadedStageImpl::unregisterRenderer(gtf::scdr::SDKHandle& sdk_)
{
    sdk_->unregisterModule("Multisample");
}
