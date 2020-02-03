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

#include "CustomTraceOutput.h"

#include <gtf/configuration/Settings.h>
#include <gtf/configuration/ScalarValue.h>
#include <gtf/configuration/PathValue.h>
#include <gtf/configuration/ScalarBuilder.h>

#define TRACE_TO_FILE_NAME "log.csv"

const uint32_t strBufferSize = 1000;

namespace traceoutputexample {

CustomTraceOutput::CustomTraceOutput()
    : stream(NULL)
{
}

void CustomTraceOutput::configure(const gtf::dependencyresolver::DependencyContainerHandle& container)
{
    if (!container.valid())
    {
        return;
    }

    gtf::configuration::SettingsHandle settings = container->get<gtf::configuration::Settings>();
    if (!settings.valid())
    {
        return;
    }

    gtf::configuration::StringScalarHandle configurationPathSetting = settings->getItem<gtf::configuration::StringScalar>("gtf.tracing.config.path");
    if (!configurationPathSetting.valid())
    {
        return;
    }

    gtf::configuration::StringScalarBuilderHandle stringBuilder = settings->getBuilder<gtf::configuration::StringScalarBuilder>();
    if (!stringBuilder.valid())
    {
        return;
    }
    stringBuilder->set(TRACE_TO_FILE_NAME);

    gtf::configuration::StringScalarHandle tracePathString = stringBuilder->build();
    if (!tracePathString.valid())
    {
        return;
    }

    gtf::configuration::PathValueHandle tracePath = tracePathString->safecast<gtf::configuration::PathValue>();
    if (!tracePath.valid())
    {
        return;
    }

    tracePath->setBasePath(configurationPathSetting->get());

    const char* path = tracePath->get();

    if (stream)
    {
        flush();
        fclose(stream);
    }
    stream = fopen(path, "w");
}

void CustomTraceOutput::trace(gtf::tracing::MessageHandle& message)
{
    if (!stream)
    {
        return;
    }

    if (!message.valid())
    {
        return;
    }

    if (message->getSeverity() == gtf::tracing::Severities::None)
    {
        return;
    }

    gtf::tracing::TraceChannelHandle traceChannelHandle = message->getChannel();
    if (!traceChannelHandle.valid())
    {
        return;
    }

    const char* traceSeverity = getSeverityName(message->getSeverity());

    const char* traceMessage = message->getMessage();

    uint64_t traceTime = message->getSystemTick();

    char outputMessage[strBufferSize];

    const char* traceChannelName = traceChannelHandle->getName();

    const uint32_t outputSize = snprintf(outputMessage, strBufferSize, "%llu, %s, %s, %s \n", (unsigned long long)traceTime, traceChannelName, traceSeverity, traceMessage);

    fwrite(outputMessage, outputSize, 1, stream);

}

void CustomTraceOutput::flush()
{
    if (stream)
    {
        fflush(stream);
    }
}

CustomTraceOutput::~CustomTraceOutput()
{
    if (stream)
    {
        fclose(stream);
    }
}

const char* CustomTraceOutput::getSeverityName(gtf::tracing::Severities::Value severity)

{
    const char* severityName = "Unknown";
    switch (severity)
    {
    case gtf::tracing::Severities::Debug:
        severityName = "Debug";
        break;
    case gtf::tracing::Severities::Info:
        severityName = "Info";
        break;
    case gtf::tracing::Severities::Notice:
        severityName = "Notice";
        break;
    case gtf::tracing::Severities::Warning:
        severityName = "Warning";
        break;
    case gtf::tracing::Severities::Error:
        severityName = "Error";
        break;
    case gtf::tracing::Severities::Fatal:
        severityName = "Fatal";
        break;
    case gtf::tracing::Severities::None:
        severityName = "None";
        break;
    }
    return severityName;
}

} // namespace traceoutputexample
