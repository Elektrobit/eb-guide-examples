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
#include <gtf/configuration/Settings.h>
#include <gtf/osal/filesystem/Path.h>
#include "DownscalingFeature.h"

GTF_PLUGIN_SO_SYMBOL const char * GTF_PLUGIN_GET_VERSION()
{
    return GTF_PLUGIN_VERSION_STRING;
}

template <class T>
static gtf::dependencyresolver::InterfaceHandle createWidgetExtension(const gtf::dependencyresolver::DependencyContainerHandle& container)
{
    if (!container)
    {
        return gtf::dependencyresolver::InterfaceHandle();
    }

    gtf::scene::TypeResolverHandle typeResolver = container->get<gtf::scene::TypeResolver>();
    if (!typeResolver)
    {
        return gtf::dependencyresolver::InterfaceHandle();
    }

    gtf::properties::PropertyObserverHandle observer = container->get<gtf::properties::PropertyObserver>();
    if (!observer)
    {
        return gtf::dependencyresolver::InterfaceHandle();

    }

    gtf::resourcesystem::FactoryHandle factory = container->get<gtf::resourcesystem::Factory>();
    if (!factory)
    {
        return gtf::dependencyresolver::InterfaceHandle();

    }

    gtf::configuration::SettingsHandle settings = container->get<gtf::configuration::Settings>();
    if (!settings)
    {
        return gtf::dependencyresolver::InterfaceHandle();
    }

    gtf::osal::filesystem::PathHandle path = container->get<gtf::osal::filesystem::Path>();
    if (!path)
    {
        return gtf::dependencyresolver::InterfaceHandle();
    }


    return gtf::dependencyresolver::InterfaceHandle(new T(typeResolver, observer, factory,settings,path));
}

GTF_PLUGIN_SO_SYMBOL void GTF_PLUGIN_EXTEND_CATALOG(void * catalog)
{
    if (!catalog)
    {
        return;
    }

    gtf::dependencyresolver::DependencyCatalog & dependencyCatalog = *static_cast<gtf::dependencyresolver::DependencyCatalog*>(catalog);
    gtf::dependencyresolver::extendCatalog<downscalingFeature::DownscalingFeature, createWidgetExtension<downscalingFeature::DownscalingFeature>>(dependencyCatalog);
}
