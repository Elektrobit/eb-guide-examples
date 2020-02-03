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

#include <gtf/dependencyresolver/DependencyContainer.h>
#include <gtf/pluginloader/PluginSymbols.h>
#include <gtf/properties/ContainerObserver.h>
#include "PreparedStageImpl.h"
#include "FocusPolicyWidgetFeatureDesc.h"
#include "RandomFocusFeature.h"

GTF_PLUGIN_SO_SYMBOL const char * GTF_PLUGIN_GET_VERSION()
{
    return GTF_PLUGIN_VERSION_STRING;
}

static gtf::dependencyresolver::InterfaceHandle createPreparedStage(const gtf::dependencyresolver::DependencyContainerHandle& container)
{
    if (!container)
    {
        return gtf::dependencyresolver::InterfaceHandle();
    }

    return gtf::dependencyresolver::InterfaceHandle(new focusedpolicywidgetfeature::PreparedStageImpl(container));
}

static gtf::dependencyresolver::InterfaceHandle CreateFocusFeature(const gtf::dependencyresolver::DependencyContainerHandle& container)
{
    if (!container.valid())
    {
        return gtf::dependencyresolver::InterfaceHandle();
    }

    gtf::scene::TypeResolverHandle resolver = container->get<gtf::scene::TypeResolver>();
    if (!resolver.valid())
    {
        return gtf::dependencyresolver::InterfaceHandle();
    }

    return gtf::dependencyresolver::InterfaceHandle(new focusedpolicywidgetfeature::RandomFocusFeature(resolver));
}

static gtf::dependencyresolver::InterfaceHandle createFocusPolicy(const gtf::dependencyresolver::DependencyContainerHandle& container)
{
    if (!container.valid())
    {
        return gtf::dependencyresolver::InterfaceHandle();
    }
    gtf::properties::ContainerObserverHandle observer = container->get<gtf::properties::ContainerObserver>();
    if (!observer)
    {
        return gtf::dependencyresolver::InterfaceHandle();
    }

    return gtf::dependencyresolver::InterfaceHandle(new focusedpolicywidgetfeature::FocusPolicyFeature(observer));
}

GTF_PLUGIN_SO_SYMBOL void GTF_PLUGIN_EXTEND_CATALOG(void * catalog)
{
    if (!catalog)
    {
        return;
    }

    gtf::dependencyresolver::DependencyCatalog& dependencyCatalog = *static_cast<gtf::dependencyresolver::DependencyCatalog*>(catalog);
    gtf::dependencyresolver::extendCatalog<gtf::model::PreparedStage, createPreparedStage>(dependencyCatalog);
    gtf::dependencyresolver::extendCatalog<gtf::focus::Features, CreateFocusFeature>(dependencyCatalog);
    gtf::dependencyresolver::extendCatalog<focusedpolicywidgetfeature::FocusPolicyFeature, createFocusPolicy>(dependencyCatalog);
}
