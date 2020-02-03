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

#include "PreparedStageImpl.h"
#include "ModelLauncherDescriptorProvider.h"

modellauncher::PreparedStageImpl::PreparedStageImpl(gtf::dependencyresolver::DependencyContainerHandle const& containerToUse)
    : container(containerToUse)
{
    descriptors = new ModelLauncherDescriptorProvider();

    container->registerInstance<gtf::metainformation::DescriptorProvider>(descriptors);
}

modellauncher::PreparedStageImpl::~PreparedStageImpl()
{
    container->unregisterInstance(descriptors);
}
