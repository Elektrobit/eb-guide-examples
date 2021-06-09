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
#include "TemperatureConverterToKelvin.h"
#include "Tracing.h"

dependencyresolvingexample::TemperatureConverterToKelvin::TemperatureConverterToKelvin()
    : running(false)
{
}

void dependencyresolvingexample::TemperatureConverterToKelvin::start()
{
    GTF_INFO(GTF_Dependency_Resolver_Example, "Starting the " << getName() << " converter.");
    running = true;
}

float dependencyresolvingexample::TemperatureConverterToKelvin::convert(float celsiusTemperatureValue)
{
    if (!isStarted())
    {
        return 0.0;
    }

    float result = celsiusTemperatureValue + static_cast<float>(273.15);

    GTF_INFO(GTF_Dependency_Resolver_Example, "Conversion to Kelvin from  Celsius: " << celsiusTemperatureValue << " C -> " << result << " K");

    return result;
}

void dependencyresolvingexample::TemperatureConverterToKelvin::shutdown()
{
    GTF_INFO(GTF_Dependency_Resolver_Example, "Shutting down the " << getName() << " converter.");
    running = false;
}

const char* dependencyresolvingexample::TemperatureConverterToKelvin::getName()
{
    return gtf::dependencyresolver::InterfaceName<TemperatureConverterToKelvin>::name();
}

bool dependencyresolvingexample::TemperatureConverterToKelvin::isStarted()
{
    return running;
}
