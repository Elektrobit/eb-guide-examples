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
#include <gtf/type/TypeId.h>
#include <gtf/script/ParameterStack.h>
#include <gtf/metainformation/ActionDescriptor.h>
#include <gtf/coreruntime/CoreModel.h>
#include <gtf/coreruntime/CoreRuntime.h>

// declare local callback function
static void exampleSum(void*, gtf::script::ParameterStack& stack);

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
    if (false == container->getAll<gtf::coreruntime::CoreRuntime>(runtimes))
    {
        return;
    }

    gtf::coreruntime::CoreModelHandle model = container->get<gtf::coreruntime::CoreModel>();
    if (false == model.valid())
    {
        return;
    }

    uint32_t actionId = model->getActionId(SUMFUNCTION);
    if (actionId == gtf::types::limits::uint32_max)
    {
        return;
    }

    for (gtf::container::Array<gtf::coreruntime::CoreRuntimeHandle>::Iterator it = runtimes.begin(); it != runtimes.end(); ++it)
    {
        gtf::coreruntime::CoreRuntimeHandle handle = *it;
        handle->setActionImplementation(actionId, gtf::metainformation::ActionDescriptor::Implementation(NULL, &exampleSum));
    }
}

static void exampleSum(void*, gtf::script::ParameterStack& stack)
{
    int32_t operand2;
    stack.Scalar_PopValue(gtf::type::TypeId::eTypeId_int32, &operand2, sizeof(operand2));

    int32_t operand1;
    stack.Scalar_PopValue(gtf::type::TypeId::eTypeId_int32, &operand1, sizeof(operand1));

    int32_t result = operand1 + operand2;

    stack.Scalar_PushValue(gtf::type::TypeId::eTypeId_int32, &result, sizeof(result));
}
