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
#ifndef GTF_CUSTOM_TRACE_OUTPUT
#define GTF_CUSTOM_TRACE_OUTPUT

#include <gtf/tracing/TraceOutput.h>

#include <cstdio>

namespace traceoutputexample {
class CustomTraceOutput : public gtf::tracing::TraceOutput
{
public:
    
    CustomTraceOutput();

    virtual void configure(const gtf::dependencyresolver::DependencyContainerHandle& container);

    virtual void trace(gtf::tracing::MessageHandle& message);

    virtual void flush();

    virtual ~CustomTraceOutput();

private:    
    const char* getSeverityName(gtf::tracing::Severities::Value severity);
    FILE* stream;
};
} // namespace traceoutputexample

#endif //GTF_CUSTOM_TRACE_OUTPUT
