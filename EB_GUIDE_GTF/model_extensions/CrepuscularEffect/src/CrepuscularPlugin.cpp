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
#include "PreparedStageImpl.h"

GTF_PLUGIN_SO_SYMBOL const char * GTF_PLUGIN_GET_VERSION()
{
    return GTF_PLUGIN_VERSION_STRING;
}

static gtf::dependencyresolver::InterfaceHandle createPreparedStage(const gtf::dependencyresolver::DependencyContainerHandle& container)
{
    return gtf::dependencyresolver::InterfaceHandle(new crepuscular::PreparedStageImpl(container));
}

static gtf::dependencyresolver::InterfaceHandle createLoadedStage(const gtf::dependencyresolver::DependencyContainerHandle& container)
{
    return gtf::dependencyresolver::InterfaceHandle(new crepuscular::LoadedStageImpl(container));
}


GTF_PLUGIN_SO_SYMBOL void GTF_PLUGIN_EXTEND_CATALOG(void * catalog)
{
    if (!catalog)
    {
        return;
    }

    gtf::dependencyresolver::DependencyCatalog& dependencyCatalog = *static_cast<gtf::dependencyresolver::DependencyCatalog*>(catalog);
    gtf::dependencyresolver::extendCatalog<gtf::model::PreparedStage, createPreparedStage>(dependencyCatalog);
    gtf::dependencyresolver::extendCatalog<gtf::model::LoadedStage, createLoadedStage>(dependencyCatalog);
}


