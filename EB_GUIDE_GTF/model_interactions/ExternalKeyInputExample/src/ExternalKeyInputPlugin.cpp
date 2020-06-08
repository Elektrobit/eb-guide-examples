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
#include <gtf/osal/MainTaskDispatcher.h>
#include <gtf/displayfactory/inputmapper/ExternalInput.h>
#include <gtf/coreruntime/CoreRuntime.h>
#include "RunningStageImpl.h"

static gtf::dependencyresolver::InterfaceHandle createEventApplication(const gtf::dependencyresolver::DependencyContainerHandle& container)
{
    gtf::dependencyresolver::InterfaceHandle defaultApplicationHandle;

    if (!container.valid())
    {
        return defaultApplicationHandle;
    }

    gtf::osal::MainTaskDispatcherHandle mainDispatcher = container->get<gtf::osal::MainTaskDispatcher>();
    if (!mainDispatcher.valid())
    {
        return defaultApplicationHandle;
    }

    gtf::coreruntime::CoreRuntimeHandle runtime = container->get<gtf::coreruntime::CoreRuntime>();
    if (!runtime.valid())
    {
        return defaultApplicationHandle;
    }

    gtf::dependencyresolver::DependencyContainerHandle runtimeContainer = runtime->getDependencyContainer();
    if (!runtimeContainer.valid())
    {
        return defaultApplicationHandle;
    }

    gtf::displayfactory::inputmapper::ExternalInputHandle externalInput = runtimeContainer->get<gtf::displayfactory::inputmapper::ExternalInput>();
    if (!externalInput.valid())
    {
        return defaultApplicationHandle;
    }

    externalkeyinputexample::RunningStageImpl* const runningStage = new externalkeyinputexample::RunningStageImpl(mainDispatcher, externalInput);

    return gtf::dependencyresolver::InterfaceHandle(runningStage);
}

GTF_PLUGIN_SO_SYMBOL const char* GTF_PLUGIN_GET_VERSION()
{
    return GTF_PLUGIN_VERSION_STRING;
}

GTF_PLUGIN_SO_SYMBOL void GTF_PLUGIN_EXTEND_CATALOG(void* catalog)
{
    if (NULL == catalog)
    {
        return;
    }

    gtf::dependencyresolver::DependencyCatalog& dependencyCatalog = *static_cast<gtf::dependencyresolver::DependencyCatalog*>(catalog);
    gtf::dependencyresolver::extendCatalog<gtf::model::RunningStage, createEventApplication>(dependencyCatalog);
}
