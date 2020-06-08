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
#include <gtf/configuration/PathValue.h>
#include <gtf/osal/MainTaskDispatcher.h>
#include <gtf/datapool/Datapool.h>
#include <gtf/configuration/Settings.h>
#include <gtf/osal/DelegateTask.h>
#include <gtf/resourcesystem/Factory.h>

#include "MandelbrotSet.h"

namespace imagedataexample {
class RunStageImpl : public gtf::launcher::RunStage
{
public:
    RunStageImpl(const gtf::dependencyresolver::DependencyContainerHandle& container);
    ~RunStageImpl();

private:
    bool getDependencies();

    void processLoop();

    void readValueAtIndex(uint32_t index);

    void initializeDatapool();

    void readAllDatapoolItems();

    void loadExternalImage();

    void loadDynamicImage();

    gtf::configuration::PathValueHandle getPathToFile(const char* configurationItem);

    gtf::dependencyresolver::DependencyContainerHandle container;
    gtf::osal::MainTaskDispatcherHandle                taskDispatcher;
    gtf::datapool::DatapoolHandle                      datapool;
    gtf::configuration::SettingsHandle                 settings;
    gtf::configuration::PathValueHandle                imagePath;

    gtf::osal::DelegateTask            processLoopTask;
    gtf::resourcesystem::FactoryHandle factory;
    MandelbrotSet                      imageCreator;
    uint32_t                           loopCounter;
};
} // namespace imagedataexample

#endif //GTF_RUN_STAGE_IMPL_H
