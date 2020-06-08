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
#include <gtf/eventsystem/ScalarValue.h>

#include "include/export/local/events/all_events.h"
#include "RunningStageImpl.h"

externaltouchinputexample::RunningStageImpl::RunningStageImpl(gtf::osal::MainTaskDispatcherHandle const& taskDispatcherToUse, gtf::displayfactory::inputmapper::ExternalInputHandle const& externalInputToUse)
    : taskDispatcher(taskDispatcherToUse)
    , externalInput(externalInputToUse)
    , xCoordinate(0)
    , step(SwipeSteps::InitialStep)
{
    gtf::utils::delegate::Delegate<void()> const loopCallback = gtf::utils::delegate::Delegate<void()>::create<RunningStageImpl, &RunningStageImpl::loopProcess>(this);
    loopTask.setExecuteCallback(loopCallback);

    uint32_t const modelLoadingDelay = 3000;
    taskDispatcher->performTaskDelayed(loopTask, modelLoadingDelay);
}

externaltouchinputexample::RunningStageImpl::~RunningStageImpl()
{
    taskDispatcher->cancelTask(loopTask);
    taskDispatcher.reset();
    externalInput.reset();
}

void externaltouchinputexample::RunningStageImpl::loopProcess()
{
    switch (step)
    {
    case SwipeSteps::InitialStep:
    {
        step = handleInitialStep();
        break;
    }
    case SwipeSteps::PressStep:
    {
        step = handlePressStep();
        break;
    }
    case SwipeSteps::DragStep:
    {
        step = handleDragStep();
        break;
    }
    case SwipeSteps::ReversePressStep:
    {
        step = handleReversePressStep();
        break;
    }
    case SwipeSteps::ReverseDragStep:
    {
        step = handleReverseDragStep();
        break;
    }
    default:
    {
        step = handleDefault();
        break;
    }
    }

    bool isPressing = (step == SwipeSteps::PressStep) || (step == SwipeSteps::ReversePressStep);
    if (isPressing)
    {
        uint32_t const touchPressDelay = 2000;
        taskDispatcher->performTaskDelayed(loopTask, touchPressDelay);
    }
    else
    {
        uint32_t const touchMoveDelay = 10;
        taskDispatcher->performTaskDelayed(loopTask, touchMoveDelay);
    }
}

externaltouchinputexample::RunningStageImpl::SwipeSteps::Value externaltouchinputexample::RunningStageImpl::handleInitialStep()
{
    sendResponseEvent(gtf::displayfactory::inputmapper::ExternalInput::TouchStatus::TouchNew, xCoordinate);
    return SwipeSteps::PressStep;
}

externaltouchinputexample::RunningStageImpl::SwipeSteps::Value externaltouchinputexample::RunningStageImpl::handlePressStep()
{
    sendResponseEvent(gtf::displayfactory::inputmapper::ExternalInput::TouchStatus::TouchPress, xCoordinate);
    return SwipeSteps::DragStep;
}

externaltouchinputexample::RunningStageImpl::SwipeSteps::Value externaltouchinputexample::RunningStageImpl::handleDragStep()
{
    if (xCoordinate < 300)
    {
        sendResponseEvent(gtf::displayfactory::inputmapper::ExternalInput::TouchStatus::TouchMove, xCoordinate);
        xCoordinate += 2;
        return SwipeSteps::DragStep;
    }

    sendResponseEvent(gtf::displayfactory::inputmapper::ExternalInput::TouchStatus::TouchRelease, xCoordinate);
    return SwipeSteps::ReversePressStep;
}

externaltouchinputexample::RunningStageImpl::SwipeSteps::Value externaltouchinputexample::RunningStageImpl::handleReversePressStep()
{
    sendResponseEvent(gtf::displayfactory::inputmapper::ExternalInput::TouchStatus::TouchPress, xCoordinate);
    return SwipeSteps::ReverseDragStep;
}

externaltouchinputexample::RunningStageImpl::SwipeSteps::Value externaltouchinputexample::RunningStageImpl::handleReverseDragStep()
{
    if (xCoordinate > 0)
    {
        sendResponseEvent(gtf::displayfactory::inputmapper::ExternalInput::TouchStatus::TouchMove, xCoordinate);
        xCoordinate -= 2;
        return SwipeSteps::ReverseDragStep;
    }

    sendResponseEvent(gtf::displayfactory::inputmapper::ExternalInput::TouchStatus::TouchRelease, xCoordinate);
    return SwipeSteps::InitialStep;
}

externaltouchinputexample::RunningStageImpl::SwipeSteps::Value externaltouchinputexample::RunningStageImpl::handleDefault()
{
    return SwipeSteps::InitialStep;
}

void externaltouchinputexample::RunningStageImpl::sendResponseEvent(gtf::displayfactory::inputmapper::ExternalInput::TouchStatus::Value status, uint32_t x)
{
    gtf::eventsystem::local::EventHandle touchInputEvent = externalInput->createTouchEvent(1, LOCAL_EVENT_MSG_ID_EXTERNALTOUCH, status, x, 0, 0);
    if (!touchInputEvent.valid())
    {
        return;
    }

    externalInput->send(touchInputEvent);
}
