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
#include <gtf/configuration/PathValue.h>
#include <gtf/configuration/Settings.h>
#include <gtf/osal/MainTaskDispatcher.h>
#include <gtf/osal/DelegateTask.h>
#include <gtf/datapool/Datapool.h>
#include <gtf/datapool/Window.h>

namespace windowlistexample {
class RunStageImpl : public gtf::launcher::RunStage
{
public:
    RunStageImpl(gtf::configuration::SettingsHandle const&  settingsToUse,
                     gtf::osal::MainTaskDispatcherHandle const& taskDispatcherToUse,
                     gtf::datapool::DatapoolHandle const&       datapoolToUse);

    ~RunStageImpl();

private:
    void initializeDatapool();

    gtf::configuration::PathValueHandle getDatapoolDescription();

    void readAllDatapoolItems();

    void processDatapoolItems();

    bool readIntegerValue(uint32_t datapoolIndex, int32_t& value);

    void readValueAtIndex(uint32_t);

    void invoker();

    void updateWindowList();

    gtf::configuration::SettingsHandle  settings;
    gtf::osal::MainTaskDispatcherHandle taskDispatcher;
    gtf::datapool::DatapoolHandle       datapool;
    gtf::osal::DelegateTask             processDatapoolTask;
    gtf::datapool::WindowHandle         windowHandle;
};
} // namespace windowlistexample

#endif //GTF_RUN_STAGE_IMPL_H
