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
#include <gtf/configuration/ListValue.h>

#include "RunStageImpl.h"
#include "Tracing.h"

namespace settingsexample {
RunStageImpl::RunStageImpl(gtf::configuration::SettingsHandle const& settingsToUse)
    : settings(settingsToUse)
{
    gtf::configuration::StringListHandle plugins = settings->getItem<gtf::configuration::StringList>("gtf.core.pluginstoload");
    if (false == plugins.valid())
    {
        return;
    }

    uint32_t const size = plugins->getSize();

    const char* plugin = NULL;

    for (uint32_t i = 0; i < size; ++i)
    {
        plugin = plugins->getElement(i);
        GTF_INFO(GTF_Settings_Example, plugin << " plugin is loaded");
    }
}

RunStageImpl::~RunStageImpl()
{
    settings.reset();
}

} // namespace settingsexample
