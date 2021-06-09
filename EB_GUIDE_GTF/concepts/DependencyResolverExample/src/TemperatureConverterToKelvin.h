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

#ifndef GTF_TEMPERATURE_CONVERTER_TO_KELVIN_H
#define GTF_TEMPERATURE_CONVERTER_TO_KELVIN_H

#include <gtf/dependencyresolver/DependencyContainer.h>

#include "TemperatureConverter.h"

namespace dependencyresolvingexample {
class TemperatureConverterToKelvin : public TemperatureConverter
{
public:
    TemperatureConverterToKelvin();

    virtual void start();

    virtual float convert(float celsiusTemperatureValue);

    virtual bool isStarted();

    virtual void shutdown();

    virtual const char* getName();

private:
    bool running;
};
} // namespace dependencyresolvingexample
GTF_DEFINE_INTERFACE_NAME(dependencyresolvingexample::TemperatureConverterToKelvin)
#endif // GTF_TEMPERATURE_CONVERTER_TO_KELVIN_H
