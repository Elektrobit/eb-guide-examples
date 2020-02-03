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
#include <gtf/coreruntime/CoreModel.h>
#include <gtf/coreruntime/CoreRuntime.h>
#include <gtf/model/ModelService.h>
#include <gtf/configuration/Settings.h>
#include <gtf/configuration/ScalarValue.h>
#include <gtf/metainformation/ActionDescriptor.h>

namespace modellauncher {
class LoadedStageImpl : public gtf::model::LoadedStage
{
public:
    LoadedStageImpl(gtf::dependencyresolver::DependencyContainerHandle const& containerToUse);

    ~LoadedStageImpl();

    void setImplementation();

    void registerFunction(gtf::coreruntime::CoreModelHandle &model, const char * functionName, const gtf::metainformation::ActionDescriptor::TActionFunction &function);

    struct ServiceInfo
    {
        gtf::model::ModelServiceHandle service;
        const char*                    platformPath;
        const char*                    modelPath;
    };

private:
    ServiceInfo                                                serviceInfo;
    gtf::configuration::StringScalarHandle                     fwpath;
    gtf::configuration::StringScalarHandle                     modelpath;
    gtf::dependencyresolver::DependencyContainerHandle         container;
    gtf::container::Array<gtf::coreruntime::CoreRuntimeHandle> runtimes;
};
} // namespace modellauncher

#endif // GTF_LOADED_STAGE_IMPL_H
