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
#include "include/export/local/events/all_events.h"
#include "RunningStageImpl.h"

#include <gtf/eventsystem/ScalarValue.h>

externalkeyinputexample::RunningStageImpl::RunningStageImpl(gtf::osal::MainTaskDispatcherHandle const&   taskDispatcherToUse,
                                                            gtf::displayfactory::inputmapper::ExternalInputHandle const& externalInputToUse)
    : taskDispatcher(taskDispatcherToUse)
    , externalInput(externalInputToUse)
    , eventCounter(0)
{
    gtf::utils::delegate::Delegate<void()> const loopCallback = gtf::utils::delegate::Delegate<void()>::create<RunningStageImpl, &RunningStageImpl::loopProcess>(this);
    // setup the call back for the continuous loop
    loopTask.setExecuteCallback(loopCallback);

    //enqueue a task to the main task dispatcher
    taskDispatcher->performTaskDelayed(loopTask, 500);
}

externalkeyinputexample::RunningStageImpl::~RunningStageImpl()
{
    taskDispatcher->cancelTask(loopTask);
    taskDispatcher.reset();
    externalInput.reset();
}

void externalkeyinputexample::RunningStageImpl::loopProcess()
{
    ++eventCounter;

    bool const isEvenEvent = eventCounter % 2 == 0;

    if (true == isEvenEvent)
    {
        sendResponseEvent(gtf::displayfactory::inputmapper::ExternalInput::KeyStatus::KeyRelease);
    }
    else
    {
        sendResponseEvent(gtf::displayfactory::inputmapper::ExternalInput::KeyStatus::KeyPress);
    }

    taskDispatcher->performTaskDelayed(loopTask, 500);
}

void externalkeyinputexample::RunningStageImpl::sendResponseEvent(gtf::displayfactory::inputmapper::ExternalInput::KeyStatus::Value status)
{
    gtf::eventsystem::local::EventHandle keyInputEvent = externalInput->createKeyEvent(0, LOCAL_EVENT_MSG_ID_KEYEVENT, status);
    if (!keyInputEvent)
    {
        return;
    }

    externalInput->send(keyInputEvent);
}
