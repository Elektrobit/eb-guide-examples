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

#include "RunStageImpl.h"
#include <gtf/configuration/Settings.h>
#include "Tracing.h"

namespace dependencyresolvingexample {

RunStageImpl::RunStageImpl(gtf::container::Array<TemperatureConverterHandle> converters_)
    : converters(converters_)
{
    GTF_INFO(GTF_Dependency_Resolver_Example, "Temperature converter application successfully started.");

    for (gtf::container::Array<TemperatureConverterHandle>::Iterator it = converters.begin(); it != converters.end(); ++it)
    {
        TemperatureConverterHandle converter = (*it);

        converter->start();

        float const temperatureValueToConvert = 35.0;

        converter->convert(temperatureValueToConvert);
    }
}

RunStageImpl::~RunStageImpl()
{
    for (gtf::container::Array<TemperatureConverterHandle>::Iterator it = converters.begin(); it != converters.end(); ++it)
    {
        TemperatureConverterHandle converter = (*it);

        converter->shutdown();
    }

    converters.clear();
    GTF_INFO(GTF_Dependency_Resolver_Example, "Temperature converter application successfully stopped.");
}

} // namespace dependencyresolvingexample
