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
#include <gtf/osal/MainTaskDispatcher.h>
#include <gtf/osal/DelegateTask.h>
#include <gtf/eventsystem/EventQueue.h>

namespace eventexample {
class RunStageImpl : public gtf::launcher::RunStage
{
public:
    RunStageImpl(gtf::osal::MainTaskDispatcherHandle const&        taskDispatcherToUse,
                     gtf::eventsystem::global::EventQueueHandle const& eventQueueToUse);

    ~RunStageImpl();

private:
    void processEvent(const gtf::eventsystem::global::EventHandle&);

    void dispatch();

    void invoker();

    void sendResponseEvent(const char* group_id, const char* msg_id);

    gtf::osal::MainTaskDispatcherHandle        taskDispatcher;
    gtf::eventsystem::global::EventQueueHandle eventQueue;
    gtf::osal::DelegateTask                    processEventTask;
    uint32_t                                   eventCounter;
};
} // namespace eventexample

#endif //GTF_RUN_STAGE_IMPL_H
