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
#include <gtf/launcher/PrepareStage.h>
#include "EarlyStage.h"
#include "EarlyStageProvider.h"
#include "LateStage.h"
#include "LateStageProvider.h"
#include "PrepareStageImpl.h"
#include "Tracing.h"

static gtf::dependencyresolver::InterfaceHandle createEarlyStageProvider(const gtf::dependencyresolver::DependencyContainerHandle&)
{
    corestageexample::EarlyStageStageProvider* earlyStageProvider = new corestageexample::EarlyStageStageProvider();

    return gtf::dependencyresolver::InterfaceHandle(earlyStageProvider);
}

template <typename T>
static gtf::dependencyresolver::InterfaceHandle createStage(const gtf::dependencyresolver::DependencyContainerHandle& container)
{
    gtf::dependencyresolver::InterfaceHandle defaultApplicationHandle;

    if (false == container.valid())
    {
        return defaultApplicationHandle;
    }
    gtf::launcher::StageHandle const stage = new T();

    return gtf::dependencyresolver::InterfaceHandle(stage);
}

static gtf::dependencyresolver::InterfaceHandle createEarlyStage(const gtf::dependencyresolver::DependencyContainerHandle& container)
{
    return createStage<corestageexample::EarlyStage>(container);
}

static gtf::dependencyresolver::InterfaceHandle createLateStageProvider(const gtf::dependencyresolver::DependencyContainerHandle&)
{
    corestageexample::LateStageProvider* lateStageProvider = new corestageexample::LateStageProvider();

    return gtf::dependencyresolver::InterfaceHandle(lateStageProvider);
}

static gtf::dependencyresolver::InterfaceHandle createLateStage(const gtf::dependencyresolver::DependencyContainerHandle& container)
{
    return createStage<corestageexample::LateStage>(container);
}

static gtf::dependencyresolver::InterfaceHandle createPreparedStageImpl(const gtf::dependencyresolver::DependencyContainerHandle& container)
{
    return createStage<corestageexample::PrepareStageImpl>(container);
}

GTF_PLUGIN_SO_SYMBOL const char* GTF_PLUGIN_GET_VERSION()
{
    return GTF_PLUGIN_VERSION_STRING;
}

GTF_PLUGIN_SO_SYMBOL void GTF_PLUGIN_EXTEND_CATALOG(void* catalog)
{
    if (false == catalog)
    {
        return;
    }

    gtf::dependencyresolver::DependencyCatalog& dependencyCatalog = *static_cast<gtf::dependencyresolver::DependencyCatalog*>(catalog);
    gtf::dependencyresolver::extendCatalog<gtf::launcher::StageProvider, createEarlyStageProvider>(dependencyCatalog);
    gtf::dependencyresolver::extendCatalog<gtf::launcher::StageProvider, createLateStageProvider>(dependencyCatalog);
    gtf::dependencyresolver::extendCatalog<gtf::launcher::PrepareStage, createPreparedStageImpl>(dependencyCatalog);
    gtf::dependencyresolver::extendCatalog<corestageexample::EarlyStage, createEarlyStage>(dependencyCatalog);
    gtf::dependencyresolver::extendCatalog<corestageexample::LateStage, createLateStage>(dependencyCatalog);
}
