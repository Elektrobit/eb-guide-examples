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
#include "LoadedStageImpl.h"
#include "ExtendedContainerWidget.h"

GTF_PLUGIN_SO_SYMBOL const char * GTF_PLUGIN_GET_VERSION()
{
    return GTF_PLUGIN_VERSION_STRING;
}



static gtf::dependencyresolver::InterfaceHandle createLoadedStage(const gtf::dependencyresolver::DependencyContainerHandle& container)
{
    return gtf::dependencyresolver::InterfaceHandle(new extendedcontainerwidget::LoadedStageImpl(container));
}

static gtf::dependencyresolver::InterfaceHandle createExtendedContainer(const gtf::dependencyresolver::DependencyContainerHandle& container)
{
    if (!container.valid())
    {
        return gtf::dependencyresolver::InterfaceHandle();
    }

    gtf::scene::TypeResolverHandle resolver = container->get<gtf::scene::TypeResolver>();
    if (!resolver)
    {
        return gtf::dependencyresolver::InterfaceHandle();
    }

    gtf::properties::PropertyObserverHandle observer = container->get<gtf::properties::PropertyObserver>();
    if (!observer)
    {
        return gtf::dependencyresolver::InterfaceHandle();
    }

    return gtf::dependencyresolver::InterfaceHandle(new extendedcontainerwidget::ExtendedContainerWidget(resolver, observer));
}

GTF_PLUGIN_SO_SYMBOL void GTF_PLUGIN_EXTEND_CATALOG(void * catalog)
{
    if (!catalog)
    {
        return;
    }

    gtf::dependencyresolver::DependencyCatalog& dependencyCatalog = *static_cast<gtf::dependencyresolver::DependencyCatalog*>(catalog);
    gtf::dependencyresolver::extendCatalog<gtf::model::LoadedStage, createLoadedStage>(dependencyCatalog);
    gtf::dependencyresolver::extendCatalog<extendedcontainerwidget::ExtendedContainerWidget, createExtendedContainer>(dependencyCatalog);
}
