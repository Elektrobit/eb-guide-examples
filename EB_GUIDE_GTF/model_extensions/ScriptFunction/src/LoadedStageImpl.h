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

#ifndef GTF_LOADED_STAGE_IMPL_H
#define GTF_LOADED_STAGE_IMPL_H

#include <gtf/model/LoadedStage.h>
#include <gtf/dependencyresolver/DependencyContainer.h>
#include <gtf/coreruntime/CoreRuntime.h>

namespace scriptfunction {
class LoadedStageImpl : public gtf::model::LoadedStage
{
public:
    LoadedStageImpl(gtf::dependencyresolver::DependencyContainerHandle const& containerToUse);

    ~LoadedStageImpl();

    void setImplementation();

private:
    gtf::dependencyresolver::DependencyContainerHandle         container;
    gtf::container::Array<gtf::coreruntime::CoreRuntimeHandle> runtimes;
};
} // namespace scriptfunction

#endif // GTF_LOADED_STAGE_IMPL_H
