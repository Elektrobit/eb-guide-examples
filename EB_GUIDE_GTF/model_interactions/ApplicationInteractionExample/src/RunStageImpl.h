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

#include <gtf/dependencyresolver/DependencyContainer.h>
#include <gtf/osal/MainTaskDispatcher.h>
#include <gtf/osal/DelegateTask.h>
#include <gtf/eventsystem/EventQueue.h>
#include <gtf/datapool/Datapool.h>
#include <gtf/datapool/ListValue.h>
#include <gtf/configuration/PathValue.h>
#include <gtf/configuration/Settings.h>
#include <gtf/launcher/RunStage.h>
#include <gtf/resourcesystem/Factory.h>

#include "MandelbrotSet.h"

namespace applicationinteractiondemoexample {
class RunStageImpl : public gtf::launcher::RunStage
{
public:
    RunStageImpl(gtf::dependencyresolver::DependencyContainerHandle const& containerToUse,
        gtf::osal::MainTaskDispatcherHandle const&                         taskDispatcherToUse,
        gtf::datapool::DatapoolHandle const&                               datapoolToUse,
        gtf::eventsystem::global::EventQueueHandle const&                  eventQueueToUse,
        gtf::configuration::SettingsHandle const&                          settingsToUse);

    ~RunStageImpl();

private:
    void processInteraction();

    void processLoop();

    void readValueAtIndex(uint32_t index);

    void invoker();

    void moveIntegerFromSentToReceivedDatapoolItem();

    bool readIntegerValue(uint32_t datapoolIndex, int32_t& value);

    bool writeIntegerValue(uint32_t datapoolIndex, int32_t value);

    void handleEvents(const gtf::eventsystem::global::EventHandle& event);

    void initializeDatapool();

    void readAllDatapoolItems();

    void updateWindowList();

    void changeBooleanValues();

    void changeIntValues();

    void changeStringValues();

    void loadExternalImage();

    void loadDynamicImage();

    gtf::configuration::PathValueHandle getPathToFile(const char* configurationItem);

    gtf::dependencyresolver::DependencyContainerHandle container;
    gtf::osal::MainTaskDispatcherHandle                taskDispatcher;
    gtf::datapool::DatapoolHandle                      datapool;
    gtf::eventsystem::global::EventQueueHandle         eventQueue;
    gtf::configuration::SettingsHandle                 settings;
    gtf::datapool::WindowHandle                        windowHandle;
    gtf::configuration::PathValueHandle                imagePath;
    gtf::osal::DelegateTask                            processInteractionTask;
    gtf::osal::DelegateTask                            processLoopTask;
    gtf::resourcesystem::FactoryHandle                 factory;
    MandelbrotSet                                      imageCreator;
    uint32_t                                           loopCounter;
};
} // namespace applicationinteractiondemoexample

#endif //GTF_RUN_STAGE_IMPL_H
