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

#ifndef GTF_PREPARED_STAGE_IMPL_H
#define GTF_PREPARED_STAGE_IMPL_H

#include <gtf/model/PreparedStage.h>
#include <gtf/dependencyresolver/DependencyContainer.h>
#include <gtf/metainformation/DescriptorProvider.h>
#include <gtf/coreruntime/CoreRuntime.h>

namespace modellauncher {
class PreparedStageImpl : public gtf::model::PreparedStage
{
public:
    PreparedStageImpl(gtf::dependencyresolver::DependencyContainerHandle const& containerToUse);

    ~PreparedStageImpl();

private:
    gtf::dependencyresolver::DependencyContainerHandle container;
    gtf::metainformation::DescriptorProviderHandle            descriptors;
};
} // namespace modellauncher

#endif // GTF_PREPARED_STAGE_IMPL_H
