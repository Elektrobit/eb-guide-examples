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
#include "LabelShapeAdder.h"

GTF_PLUGIN_SO_SYMBOL const char * GTF_PLUGIN_GET_VERSION()
{
    return GTF_PLUGIN_VERSION_STRING;
}

template <typename T>
static dependencyresolver::InterfaceHandle createGenericWithResolver(const dependencyresolver::DependencyContainerHandle& container_)
{
    if (!container_.valid())
    {
        return dependencyresolver::InterfaceHandle();
    }

    scene::TypeResolverHandle resolver = container_->get<scene::TypeResolver>();
    if (!resolver)
    {
        return dependencyresolver::InterfaceHandle();
    }

    return dependencyresolver::InterfaceHandle(new T(resolver));
}

GTF_PLUGIN_SO_SYMBOL void GTF_PLUGIN_EXTEND_CATALOG(void * catalog)
{
	if (!catalog)
	{
		return;
	}

	gtf::dependencyresolver::DependencyCatalog& dependencyCatalog = *static_cast<gtf::dependencyresolver::DependencyCatalog*>(catalog);
	gtf::dependencyresolver::extendCatalog<labelshapewidgetfeature::SineCurveShapeAdder, createGenericWithResolver<labelshapewidgetfeature::SineCurveShapeAdder>>(dependencyCatalog);
}
