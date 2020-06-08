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
#include <gtf/configuration/PathValue.h>
#include <gtf/configuration/ScalarValue.h>

namespace configurationimporterexample {
RunStageImpl::RunStageImpl(gtf::configuration::SettingsHandle const& settingsToUse, gtf::configurationimporter::ConfigurationImporterHandle const& configurationImporterToUse)
    : settings(settingsToUse)
    , configurationImporter(configurationImporterToUse)
{
    gtf::configuration::StringScalarHandle modelPath = settings->getItem<gtf::configuration::StringScalar>("gtf.model.default.path");
    if (!modelPath.valid())
    {
        return;
    }

    gtf::configuration::PathValueHandle customConfigurationPath = settings->getItem<gtf::configuration::PathValue>("customConfiguration");
    if (!customConfigurationPath.valid())
    {
        return;
    }

    customConfigurationPath->setBasePath(modelPath->get());

    configurationImporter->parseFile(customConfigurationPath->get());
}

RunStageImpl::~RunStageImpl()
{
    settings.reset();
}

} // namespace configurationimporterexample
