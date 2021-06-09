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

#include "LoadedStageImpl.h"
#include "ScriptFunctionNames.h"
#include <gtf/coreruntime/CoreModel.h>
#include <gtf/coreruntime/CoreRuntime.h>
#include <gtf/coreruntime/RegisterFunction.h>

using namespace gtf::script;

static int32_t exampleSum(int32_t a, int32_t b)
{
    return a + b;
}

scriptfunction::LoadedStageImpl::LoadedStageImpl(gtf::dependencyresolver::DependencyContainerHandle const& containerToUse)
    : container(containerToUse)
{
    setImplementation();
}

scriptfunction::LoadedStageImpl::~LoadedStageImpl()
{
}

void scriptfunction::LoadedStageImpl::setImplementation()
{
    if (!container->getAll<gtf::coreruntime::CoreRuntime>(runtimes))
    {
        return;
    }

    gtf::coreruntime::CoreModelHandle model = container->get<gtf::coreruntime::CoreModel>();
    if (!model.valid())
    {
        return;
    }

    for (gtf::container::Array<gtf::coreruntime::CoreRuntimeHandle>::Iterator it = runtimes.begin(); it != runtimes.end(); ++it)
    {
        gtf::coreruntime::CoreRuntimeHandle runtime = *it;
        ActionRegistry& registry = runtime->getActionRegistry();

        gtf::coreruntime::registerFunction<BindingType::Integer(BindingType::Integer, BindingType::Integer)>(*model, registry, SUMFUNCTION, gtf::utils::delegate::Delegate<int32_t(int32_t, int32_t)>::create<&exampleSum>());
    }
}

