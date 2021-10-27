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

#ifndef GTF_RUNNING_STAGE_IMPL_H
#define GTF_RUNNING_STAGE_IMPL_H

#include <gtf/osal/MainTaskDispatcher.h>
#include <gtf/osal/DelegateTask.h>
#include <gtf/model/RunningStage.h>
#include <gtf/displayfactory/inputmapper/ExternalInput.h>

namespace externalkeyinputexample {
class RunningStageImpl : public gtf::model::RunningStage
{
public:
    RunningStageImpl(gtf::osal::MainTaskDispatcherHandle const& taskDispatcherToUse, gtf::displayfactory::inputmapper::ExternalInputHandle const& externalInputToUse);

    virtual ~RunningStageImpl();

private:
    void loopProcess();

    void sendResponseEvent(gtf::displayfactory::inputmapper::ExternalInput::KeyStatus::Value status);

    gtf::osal::MainTaskDispatcherHandle   taskDispatcher;
    gtf::displayfactory::inputmapper::ExternalInputHandle externalInput;
    gtf::osal::DelegateTask               loopTask;
    uint32_t                              eventCounter;
};
} // namespace externalkeyinputexample

#endif // GTF_RUNNING_STAGE_IMPL_H
