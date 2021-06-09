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
#include "include/export/global/events/group_global.h"

namespace eventexample {
RunStageImpl::RunStageImpl(gtf::osal::MainTaskDispatcherHandle const&        taskDispatcherToUse,
                                   gtf::eventsystem::global::EventQueueHandle const& eventQueueToUse)
    : taskDispatcher(taskDispatcherToUse)
    , eventQueue(eventQueueToUse)
    , eventCounter(0)
{
    // setup the event processing task
    gtf::utils::delegate::Delegate<void()> const proccessEventCallback = gtf::utils::delegate::Delegate<void()>::create<RunStageImpl, &RunStageImpl::dispatch>(this);
    processEventTask.setExecuteCallback(proccessEventCallback);

    //unsubscrie for the event and specify the callback method
    gtf::eventsystem::global::EventQueue::Subscription const subscriberCallback = gtf::eventsystem::global::EventQueue::Subscription::create<RunStageImpl, &RunStageImpl::processEvent>(this);
    eventQueue->subscribe(gtf::eventsystem::SubscriptionType::Event, GLOBAL_EVENT_GROUP_GLOBAL, GLOBAL_EVENT_MSG_ID_WHATTODO, this, subscriberCallback);

    //set a task to listen for the subscribed event in the event queue
    gtf::utils::delegate::Delegate<void()> const invokerCallback = gtf::utils::delegate::Delegate<void()>::create<RunStageImpl, &RunStageImpl::invoker>(this);
    eventQueue->setInvoker(invokerCallback);
}

RunStageImpl::~RunStageImpl()
{
    eventQueue->unsubscribe(gtf::eventsystem::SubscriptionType::Event, GLOBAL_EVENT_GROUP_GLOBAL, GLOBAL_EVENT_MSG_ID_WHATTODO, this);
    eventQueue.reset();

    taskDispatcher->cancelTask(processEventTask);
    taskDispatcher.reset();
}

void RunStageImpl::dispatch()
{
    eventQueue->dispatch();
}

void RunStageImpl::invoker()
{
    //start the event processing task
    taskDispatcher->performTask(processEventTask);
}

void RunStageImpl::sendResponseEvent(const char* group_id, const char* msg_id)
{
    gtf::eventsystem::global::EventHandle returnEvent = eventQueue->createEvent(group_id, msg_id);
    if (!returnEvent.valid())
    {
        return;
    }

    eventQueue->sendEvent(returnEvent);
}

void RunStageImpl::processEvent(const gtf::eventsystem::global::EventHandle&)
{
    eventCounter++;

    bool const isEvenEvent = eventCounter % 2 == 0;

    if (true == isEvenEvent)
    {
        sendResponseEvent(GLOBAL_EVENT_GROUP_GLOBAL, GLOBAL_EVENT_MSG_ID_GOTOA);
    }
    else
    {
        sendResponseEvent(GLOBAL_EVENT_GROUP_GLOBAL, GLOBAL_EVENT_MSG_ID_GOTOB);
    }
}

} // namespace eventexample
