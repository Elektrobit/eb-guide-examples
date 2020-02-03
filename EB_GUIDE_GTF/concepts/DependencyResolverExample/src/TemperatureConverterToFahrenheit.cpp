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
#include "TemperatureConverterToFahrenheit.h"
#include "Tracing.h"

dependencyresolvingexample::TemperatureConverterToFahrenheit::TemperatureConverterToFahrenheit()
    : running(false)
{
}

void dependencyresolvingexample::TemperatureConverterToFahrenheit::start()
{
    GTF_INFO(GTF_Dependency_Resolver_Example, "Starting the " << getName() << " converter.");

    running = true;
}

float dependencyresolvingexample::TemperatureConverterToFahrenheit::convert(float celsiusTemperatureValue)
{
    if (false == isStarted())
    {
        return 0.0;
    }

    float result = celsiusTemperatureValue * 9 / 5 + 32;

    GTF_INFO(GTF_Dependency_Resolver_Example, "Conversion to Fahrenheit from  Celsius: " << celsiusTemperatureValue << " C -> " << result << " F");

    return result;
}

void dependencyresolvingexample::TemperatureConverterToFahrenheit::shutdown()
{
    GTF_INFO(GTF_Dependency_Resolver_Example, "Shutting down the " << getName() << " converter.");

    running = false;
}

const char* dependencyresolvingexample::TemperatureConverterToFahrenheit::getName()
{
    return gtf::dependencyresolver::InterfaceName<TemperatureConverterToFahrenheit>::name();
}

bool dependencyresolvingexample::TemperatureConverterToFahrenheit::isStarted()
{
    return running;
}
