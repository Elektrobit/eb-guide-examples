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
#include "PreparedStageImpl.h"
#include "BezierCurve.h"

template <typename T>
static gtf::dependencyresolver::InterfaceHandle createStage(const gtf::dependencyresolver::DependencyContainerHandle& container)
{
    gtf::dependencyresolver::InterfaceHandle defaultApplicationHandle;

    if (!container.valid())
    {
        return defaultApplicationHandle;
    }

    gtf::model::StageHandle const stage = new T(container);

    return gtf::dependencyresolver::InterfaceHandle(stage);
}

template <typename T>
static gtf::dependencyresolver::InterfaceHandle createCurve(const gtf::dependencyresolver::DependencyContainerHandle& container)
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

static gtf::dependencyresolver::InterfaceHandle createPreparedStageImpl(const gtf::dependencyresolver::DependencyContainerHandle& container)
{
    return createStage<bezierCurve::PreparedStageImpl>(container);
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
    gtf::dependencyresolver::extendCatalog<gtf::model::PreparedStage, createPreparedStageImpl>(dependencyCatalog);

    gtf::dependencyresolver::extendCatalog<bezierCurve::BezierCurveColor, createCurve<bezierCurve::BezierCurveColor>>(dependencyCatalog);
    gtf::dependencyresolver::extendCatalog<bezierCurve::BezierCurveFloat, createCurve<bezierCurve::BezierCurveFloat> >(dependencyCatalog);
    gtf::dependencyresolver::extendCatalog<bezierCurve::BezierCurveInteger, createCurve<bezierCurve::BezierCurveInteger>>(dependencyCatalog);
}
