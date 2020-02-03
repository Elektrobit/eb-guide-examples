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
#include "ModelLauncherNames.h"
#include <gtf/type/TypeId.h>
#include <gtf/script/ParameterStack.h>
#include <stdio.h>
#include <string.h>

// declare local callback function
static void startModel(void*, gtf::script::ParameterStack& stack);
static void stopModel(void*, gtf::script::ParameterStack& stack);

modellauncher::LoadedStageImpl::LoadedStageImpl(gtf::dependencyresolver::DependencyContainerHandle const& containerToUse)
    : container(containerToUse)
{
    serviceInfo.service = container->get<gtf::model::ModelService>();

    gtf::configuration::SettingsHandle settings = container->get<gtf::configuration::Settings>();

    if (!settings)
    {
        return;
    }

    gtf::configuration::BooleanScalarHandle isEditMode = settings->getItem<gtf::configuration::BooleanScalar>("gtf.launcher.editmode");

    if (isEditMode && isEditMode->get() == true)
    {
        return;
    }

    fwpath = settings->getItem<gtf::configuration::StringScalar>("gtf.framework.path");

    if (!fwpath)
    {
        return;
    }

    modelpath = settings->getItem<gtf::configuration::StringScalar>("gtf.model.default.path");

    if (!modelpath)
    {
        return;
    }

    serviceInfo.platformPath = fwpath->get();
    serviceInfo.modelPath = modelpath->get();

    if (!serviceInfo.service)
    {
        return;
    }

    setImplementation();
}

modellauncher::LoadedStageImpl::~LoadedStageImpl()
{
    fwpath.reset();
    serviceInfo.service.reset();
}

void modellauncher::LoadedStageImpl::setImplementation()
{
    if (false == container->getAll<gtf::coreruntime::CoreRuntime>(runtimes))
    {
        return;
    }

    gtf::coreruntime::CoreModelHandle model = container->get<gtf::coreruntime::CoreModel>();
    if (!model)
    {
        return;
    }

    registerFunction(model, STARTMODELFUNCTION, &startModel);
    registerFunction(model, STOPMODELFUNCTION, &stopModel);
}

void modellauncher::LoadedStageImpl::registerFunction(gtf::coreruntime::CoreModelHandle &model, const char * functionName, const gtf::metainformation::ActionDescriptor::TActionFunction &function)
{
    uint32_t actionId = model->getActionId(functionName);
    if (actionId == gtf::types::limits::uint32_max)
    {
        return;
    }

    for (gtf::container::Array<gtf::coreruntime::CoreRuntimeHandle>::Iterator it = runtimes.begin(); it != runtimes.end(); ++it)
    {
        gtf::coreruntime::CoreRuntimeHandle handle = *it;
        handle->setActionImplementation(actionId, gtf::metainformation::ActionDescriptor::Implementation(&serviceInfo, function));
    }
}

static void startModel(void* object, gtf::script::ParameterStack& stack)
{
    const modellauncher::LoadedStageImpl::ServiceInfo* info = static_cast<modellauncher::LoadedStageImpl::ServiceInfo*>(object);
    uint32_t length = 0;

    if (!stack.Scalar_GetValueSize(stack.GetLength() - 1, gtf::type::TypeId::eTypeId_string, length))
    {
        return;
    }

    if (length == 0)
    {
        return;
    }

    const uint32_t modelPathSize = length + 1;
    gtf::container::Array<char> modelPath;
    modelPath.resize(modelPathSize);

    if (!stack.Scalar_PopValue(gtf::type::TypeId::eTypeId_string, modelPath.getData(), length+1))
    {
        return;
    }

    const uint32_t pathSize = static_cast<uint32_t>(length + strlen(info->modelPath) + strlen("/../") + strlen("/model.json") + 1);
    gtf::container::Array<char> path;
    path.resize(pathSize);

    snprintf(path.getData(), path.size(), "%s%s%s%s", info->modelPath, "/../", modelPath.getData(), "/model.json");

    gtf::model::ModelIdentifier identifier = info->service->startModel(info->platformPath, path.getData(), gtf::model::ContainerSetup());
    stack.Scalar_PushValue(gtf::type::TypeId::eTypeId_int32, &identifier, sizeof(identifier));
}

static void stopModel(void* object, gtf::script::ParameterStack& stack)
{
    const modellauncher::LoadedStageImpl::ServiceInfo* info = static_cast<modellauncher::LoadedStageImpl::ServiceInfo*>(object);

    gtf::model::ModelIdentifier modelIdentifier;
    if (!stack.Scalar_PopValue(gtf::type::TypeId::eTypeId_int32, &modelIdentifier, sizeof(modelIdentifier)))
    {
        return;
    }

    info->service->stopModel(modelIdentifier);
}
