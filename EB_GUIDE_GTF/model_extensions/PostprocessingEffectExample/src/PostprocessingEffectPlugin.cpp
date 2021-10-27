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


#include "PostprocessingExampleEffectFactory.h"

#include <gtf/dependencyresolver/DependencyContainer.h>
#include <gtf/pluginloader/PluginSymbols.h>


GTF_PLUGIN_SO_SYMBOL const char* GTF_PLUGIN_GET_VERSION()
{
    return GTF_PLUGIN_VERSION_STRING;
}

template <typename T>
static gtf::dependencyresolver::InterfaceHandle createGenericOnlyResolver(const gtf::dependencyresolver::DependencyContainerHandle& container)
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

    return gtf::dependencyresolver::InterfaceHandle(new T(resolver));
}

GTF_PLUGIN_SO_SYMBOL void GTF_PLUGIN_EXTEND_CATALOG(void* catalog)
{
    if (!catalog)
    {
        return;
    }

    gtf::dependencyresolver::DependencyCatalog& dependencyCatalog = *static_cast<gtf::dependencyresolver::DependencyCatalog*>(catalog);
    gtf::dependencyresolver::extendCatalog<gtf::scdr::PostprocessingEffectFactory, createGenericOnlyResolver<PostprocessingExampleEffectFactory> >(dependencyCatalog);
}
