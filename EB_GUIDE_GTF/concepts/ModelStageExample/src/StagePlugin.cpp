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
#include <gtf/model/PreparedStage.h>
#include "EarlyStage.h"
#include "EarlyStageProvider.h"
#include "LateStage.h"
#include "LateStageProvider.h"
#include "PreparedStageImpl.h"

static gtf::dependencyresolver::InterfaceHandle createEarlyStageProvider(const gtf::dependencyresolver::DependencyContainerHandle&)
{
    stageexample::EarlyStageStageProvider* earlyStageProvider = new stageexample::EarlyStageStageProvider();

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

    gtf::model::StageHandle const stage = new T();

    return gtf::dependencyresolver::InterfaceHandle(stage);
}

static gtf::dependencyresolver::InterfaceHandle createEarlyStage(const gtf::dependencyresolver::DependencyContainerHandle& container)
{
    return createStage<stageexample::EarlyStage>(container);
}

static gtf::dependencyresolver::InterfaceHandle createLateStageProvider(const gtf::dependencyresolver::DependencyContainerHandle&)
{
    stageexample::LateStageProvider* lateStageProvider = new stageexample::LateStageProvider();

    return gtf::dependencyresolver::InterfaceHandle(lateStageProvider);
}

static gtf::dependencyresolver::InterfaceHandle createLateStage(const gtf::dependencyresolver::DependencyContainerHandle& container)
{
    return createStage<stageexample::LateStage>(container);
}

static gtf::dependencyresolver::InterfaceHandle createPreparedStageImpl(const gtf::dependencyresolver::DependencyContainerHandle& container)
{
    return createStage<stageexample::PreparedStageImpl>(container);
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
    gtf::dependencyresolver::extendCatalog<gtf::model::StageProvider, createEarlyStageProvider>(dependencyCatalog);
    gtf::dependencyresolver::extendCatalog<gtf::model::StageProvider, createLateStageProvider>(dependencyCatalog);
    gtf::dependencyresolver::extendCatalog<gtf::model::PreparedStage, createPreparedStageImpl>(dependencyCatalog);
    gtf::dependencyresolver::extendCatalog<stageexample::EarlyStage, createEarlyStage>(dependencyCatalog);
    gtf::dependencyresolver::extendCatalog<stageexample::LateStage, createLateStage>(dependencyCatalog);
}
