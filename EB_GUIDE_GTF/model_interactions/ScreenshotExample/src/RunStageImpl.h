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

#ifndef GTF_SCREENSHOT_APP_H
#define GTF_SCREENSHOT_APP_H

#include "RunStageImpl.h"

#include <gtf/dependencyresolver/DependencyContainer.h>
#include <gtf/osal/MainTaskDispatcher.h>
#include <gtf/osal/DelegateTask.h>
#include <gtf/launcher/RunStage.h>
#include "Screenshot.h"


namespace screenshot {

class RunStageImpl : public gtf::launcher::RunStage
{
public:
    RunStageImpl(const gtf::dependencyresolver::DependencyContainerHandle& container_);

    ~RunStageImpl();

private:
    void takeScreenShot();

    gtf::dependencyresolver::DependencyContainerHandle container;

    gtf::osal::MainTaskDispatcherHandle taskDispatcher;

    ScreenshotHandle sceneCapture;
    gtf::osal::DelegateTask processLoopTask;
};

} // namespace screenshot

#endif // GTF_SCREENSHOT_APP_H
