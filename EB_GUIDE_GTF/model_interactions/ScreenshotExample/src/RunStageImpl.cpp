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

#include <gtf/dependencyresolver/DependencyContainer.h>
#include <gtf/osal/MainTaskDispatcher.h>
#include <gtf/osal/DelegateTask.h>
#include <gtf/configuration/Settings.h>
#include <gtf/configuration/ScalarValue.h>
#include <gtf/configuration/PathValue.h>

namespace screenshot {

RunStageImpl::RunStageImpl(const gtf::dependencyresolver::DependencyContainerHandle& container_)
    : container(container_)
    , sceneCapture(new Screenshot(0))
{
    container->registerInstance<gtf::scenecapture::Output>(sceneCapture);

    taskDispatcher = container->get<gtf::osal::MainTaskDispatcher>();

    if (taskDispatcher.valid())
    {
        // setup the loop processing task
        gtf::utils::delegate::Delegate<void()> const loopCallback = gtf::utils::delegate::Delegate<void()>::create<RunStageImpl, &RunStageImpl::takeScreenShot>(this);
        processLoopTask.setExecuteCallback(loopCallback);

        taskDispatcher->performTaskDelayed(processLoopTask, 1000);
    }
}

RunStageImpl::~RunStageImpl()
{
    sceneCapture.reset();

    if (taskDispatcher.valid())
    {
        taskDispatcher->cancelTask(processLoopTask);
        taskDispatcher.reset();
    }
}

void RunStageImpl::takeScreenShot()
{
    if (sceneCapture.valid())
    {
        gtf::configuration::SettingsHandle settings = container->get<gtf::configuration::Settings>();
        if (!settings.valid())
        {
            return;
        }

        gtf::configuration::StringScalarHandle modelPath = settings->getItem<gtf::configuration::StringScalar>("gtf.model.default.path");
        if (!modelPath.valid())
        {
            return;
        }

        gtf::configuration::PathValueHandle scenePath = settings->getItem<gtf::configuration::PathValue>("screenshotName");
        if (!scenePath.valid())
        {
            return;
        }

        scenePath->setBasePath(modelPath->get());

        sceneCapture->activateScreenshot(scenePath->get());
    }

    taskDispatcher->performTaskDelayed(processLoopTask, 1000);
}

} // namespace screenshot
