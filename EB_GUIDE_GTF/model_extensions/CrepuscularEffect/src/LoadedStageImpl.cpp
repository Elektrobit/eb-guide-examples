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

#include <gtf/metainformation/DescriptorProvider.h>
#include <gtf/metainformation/PropertyDescriptor.h>
#include <gtf/metainformation/WidgetDescriptor.h>

#include "CrepuscularRenderer.h"
#include "LoadedStageImpl.h"
#include "PreparedStageImpl.h"

crepuscular::LoadedStageImpl::LoadedStageImpl(const gtf::dependencyresolver::DependencyContainerHandle& container_)
    : container(container_)
{
    container->getAll<gtf::scdr::SDK>(sdkHandles);

    for (gtf::container::Array<gtf::scdr::SDKHandle>::Iterator it = sdkHandles.begin(), e = sdkHandles.end(); it != e; ++it)
    {
        // Register the custom renderer at the SCDR
        registerRenderer(*it);
    }
}

crepuscular::LoadedStageImpl::~LoadedStageImpl()
{
    for (gtf::container::Array<gtf::scdr::SDKHandle>::Iterator it = sdkHandles.begin(), e = sdkHandles.end(); it != e; ++it)
    {
        unregisterRenderer(*it);
    }
    sdkHandles.clear();
}

bool crepuscular::LoadedStageImpl::registerRenderer(gtf::scdr::SDKHandle& sdk_)
{
    if (sdk_->getGraphicsApi() == gtf::scdr::GraphicsApi::OpenGLES20)
    {
        return false;
    }

    gtf::scdr::ModuleDescriptor desc;
    desc.className       = CREPUSCULAR_PLUGIN_NAME;
    desc.createCallback  = CrepuscularRenderer::createInstance ;
    desc.destroyCallback = CrepuscularRenderer::destroyInstance;
    desc.createParams    = &sdk_;

    return sdk_->registerModule(desc);
}

void crepuscular::LoadedStageImpl::unregisterRenderer(gtf::scdr::SDKHandle& sdk_)
{
    sdk_->unregisterModule(CREPUSCULAR_PLUGIN_NAME);
}
