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
#include "DownscalingFeatureDesc.h"


downscalingFeature::PreparedStageImpl::PreparedStageImpl(const gtf::dependencyresolver::DependencyContainerHandle& container_)
    : container(container_)
{

    featureDescriptor = new DownscalingFeatureDesc();

    container->registerInstance<gtf::metainformation::DescriptorProvider>(featureDescriptor);
}

downscalingFeature::PreparedStageImpl::~PreparedStageImpl()
{
    container->unregisterInstance(featureDescriptor);
}
