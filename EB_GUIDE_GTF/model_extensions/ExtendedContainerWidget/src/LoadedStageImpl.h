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

#ifndef GTF_EXTENDED_CONTAINER_LOADED_STAGE_IMPL_H_INCLUDED
#define GTF_EXTENDED_CONTAINER_LOADED_STAGE_IMPL_H_INCLUDED

#include <gtf/dependencyresolver/DependencyContainer.h>

#include <gtf/model/LoadedStage.h>

#include <gtf/scdr/SDK.h>

namespace extendedcontainerwidget
{
    class LoadedStageImpl : public gtf::model::LoadedStage
    {
    public:
        LoadedStageImpl(const gtf::dependencyresolver::DependencyContainerHandle& container_);
        ~LoadedStageImpl();
    private:
        gtf::dependencyresolver::DependencyContainerHandle container;
        bool registerRenderer(gtf::scdr::SDKHandle& sdk_);
        void unregisterRenderer(gtf::scdr::SDKHandle& sdk_);

        gtf::container::Array<gtf::scdr::SDKHandle> sdkHandles;
    };
}

#endif // GTF_EXTENDED_CONTAINER_LOADED_STAGE_IMPL_H_INCLUDED
