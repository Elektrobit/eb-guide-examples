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
#include <gtf/pluginloader/PluginData.h>
#include <gtf/launcher/RunStage.h>

#include "TemperatureConverterToKelvin.h"
#include "TemperatureConverterToFahrenheit.h"
#include "TemperatureConverter.h"
#include "RunStageImpl.h"

template <typename T>
static gtf::dependencyresolver::InterfaceHandle createConverter(const gtf::dependencyresolver::DependencyContainerHandle& container)
{
    gtf::dependencyresolver::InterfaceHandle defaultApplicationHandle;

    if (false == container.valid())
    {
        return defaultApplicationHandle;
    }
    dependencyresolvingexample::TemperatureConverterHandle const converter = new T();

    return gtf::dependencyresolver::InterfaceHandle(converter);
}

static gtf::dependencyresolver::InterfaceHandle createTemperatureConverterToFahrenheit(const gtf::dependencyresolver::DependencyContainerHandle& container)
{
    return createConverter<dependencyresolvingexample::TemperatureConverterToFahrenheit>(container);
}

static gtf::dependencyresolver::InterfaceHandle createTemperatureConverterToKelvin(const gtf::dependencyresolver::DependencyContainerHandle& container)
{
    return createConverter<dependencyresolvingexample::TemperatureConverterToKelvin>(container);
}

GTF_PLUGIN_SO_SYMBOL const char* GTF_PLUGIN_GET_VERSION()
{
    return GTF_PLUGIN_VERSION_STRING;
}

template <class T>
static gtf::dependencyresolver::InterfaceHandle createStage(const gtf::dependencyresolver::DependencyContainerHandle& container)
{
    if (false == container.valid())
    {
        return gtf::dependencyresolver::InterfaceHandle();
    }

    gtf::container::Array<dependencyresolvingexample::TemperatureConverterHandle> converters;

    if (false == container->getAll<dependencyresolvingexample::TemperatureConverter>(converters))
    {
        return gtf::dependencyresolver::InterfaceHandle();
    }

    return gtf::dependencyresolver::InterfaceHandle(new T(converters));
}

GTF_PLUGIN_SO_SYMBOL void GTF_PLUGIN_EXTEND_CATALOG(void* catalog)
{
    if (NULL == catalog)
    {
        return;
    }

    gtf::dependencyresolver::DependencyCatalog& dependencyCatalog = *static_cast<gtf::dependencyresolver::DependencyCatalog*>(catalog);
    gtf::dependencyresolver::extendCatalog<dependencyresolvingexample::TemperatureConverter, createTemperatureConverterToFahrenheit>(dependencyCatalog);
    gtf::dependencyresolver::extendCatalog<dependencyresolvingexample::TemperatureConverter, createTemperatureConverterToKelvin>(dependencyCatalog);
    gtf::dependencyresolver::extendCatalog<gtf::launcher::RunStage, createStage<dependencyresolvingexample::RunStageImpl> >(dependencyCatalog);
}
