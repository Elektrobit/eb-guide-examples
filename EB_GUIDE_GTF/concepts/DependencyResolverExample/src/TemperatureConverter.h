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

#ifndef GTF_TEMPERATURE_CONVERTER_H
#define GTF_TEMPERATURE_CONVERTER_H

namespace dependencyresolvingexample {
class TemperatureConverter : public gtf::dependencyresolver::Interface
{
public:
    virtual void start() = 0;

    virtual float convert(float celsiusTemperatureValue) = 0;

    virtual void shutdown() = 0;

    virtual bool isStarted() = 0;

    virtual const char* getName() = 0;
};

typedef gtf::utils::smartptr::RefCountedPtr<TemperatureConverter> TemperatureConverterHandle;

} // namespace dependencyresolvingexample
GTF_DEFINE_INTERFACE_NAME(dependencyresolvingexample::TemperatureConverter)
#endif // GTF_TEMPERATURE_CONVERTER_H
