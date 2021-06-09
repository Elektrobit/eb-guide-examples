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

#ifndef GTF_RUN_STAGE_IMPL_H
#define GTF_RUN_STAGE_IMPL_H

#include <gtf/launcher/RunStage.h>
#include <gtf/dependencyresolver/DependencyContainer.h>
#include <gtf/configuration/Settings.h>
#include <gtf/configurationimporter/ConfigurationImporter.h>

namespace configurationimporterexample {
class RunStageImpl : public gtf::launcher::RunStage
{
public:
    RunStageImpl(gtf::configuration::SettingsHandle const& settingsToUse, gtf::configurationimporter::ConfigurationImporterHandle const& configurationImporterToUse);
    ~RunStageImpl();

private:
    gtf::configuration::SettingsHandle                      settings;
    gtf::configurationimporter::ConfigurationImporterHandle configurationImporter;
};
} // namespace configurationimporterexample

#endif //GTF_RUN_STAGE_IMPL_H
