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
#include <gtf/launcher/RunStage.h>
#include "RunStageImpl.h"

GTF_PLUGIN_SO_SYMBOL const char* GTF_PLUGIN_GET_VERSION()
{
    return GTF_PLUGIN_VERSION_STRING;
}

template <class T>
static gtf::dependencyresolver::InterfaceHandle createStage(const gtf::dependencyresolver::DependencyContainerHandle& container)
{
    if (!container.valid())
    {
        return gtf::dependencyresolver::InterfaceHandle();
    }

    gtf::osal::MainTaskDispatcherHandle mainDispatcher = container->get<gtf::osal::MainTaskDispatcher>();
    if (!mainDispatcher.valid())
    {
        return gtf::dependencyresolver::InterfaceHandle();
    }

    gtf::datapool::DatapoolHandle datapool = container->get<gtf::datapool::Datapool>();
    if (!datapool.valid())
    {
        return gtf::dependencyresolver::InterfaceHandle();
    }

    gtf::configuration::SettingsHandle settings = container->get<gtf::configuration::Settings>();
    if (!settings.valid())
    {
        return gtf::dependencyresolver::InterfaceHandle();
    }

    return gtf::dependencyresolver::InterfaceHandle(new T(settings, mainDispatcher, datapool));
}

GTF_PLUGIN_SO_SYMBOL void GTF_PLUGIN_EXTEND_CATALOG(void* catalog)
{
    if (NULL == catalog)
    {
        return;
    }

    gtf::dependencyresolver::DependencyCatalog& dependencyCatalog = *static_cast<gtf::dependencyresolver::DependencyCatalog*>(catalog);
    gtf::dependencyresolver::extendCatalog<gtf::launcher::RunStage, createStage<datapoolexample::RunStageImpl>>(dependencyCatalog);
}
