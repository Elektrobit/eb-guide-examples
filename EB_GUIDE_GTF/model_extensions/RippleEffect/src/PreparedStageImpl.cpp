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
#include "RippleEffectDesc.h"

rippleeffect::PreparedStageImpl::PreparedStageImpl(const gtf::dependencyresolver::DependencyContainerHandle& container_)
: container(container_)
{
    descriptors = new RippleEffectDesc();

    container->registerInstance<gtf::metainformation::DescriptorProvider>(descriptors);
}

rippleeffect::PreparedStageImpl::~PreparedStageImpl()
{
    container->unregisterInstance(descriptors);
}
