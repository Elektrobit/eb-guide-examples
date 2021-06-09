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
#include <gtf/pluginloader/PluginSymbols.h>

using namespace gtf::tracing;

extern "C" GTF_PLUGIN_SO_SYMBOL TraceOutput* TRACE_OUTPUT_FACTORY_FUNCTION_CALL(void)
{
    static traceoutputexample::CustomTraceOutput s_fileOut;
    return &s_fileOut;
}
