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

#ifndef GTF_LABEL_SHAPE_WIDGET_FEATURE_PREPARED_STAGE_IMPL_H
#define GTF_LABEL_SHAPE_WIDGET_FEATURE_PREPARED_STAGE_IMPL_H

#include <gtf/dependencyresolver/DependencyContainer.h>
#include <gtf/metainformation/DescriptorProvider.h>
#include <gtf/model/PreparedStage.h>

namespace labelshapewidgetfeature {
class PreparedStageImpl : public gtf::model::PreparedStage
{
public:
    PreparedStageImpl(const gtf::dependencyresolver::DependencyContainerHandle& container_);
    virtual ~PreparedStageImpl();

private:
    gtf::dependencyresolver::DependencyContainerHandle container;

    gtf::metainformation::DescriptorProviderHandle descriptors;

};
} // namespace labelshapewidgetfeature

#endif // GTF_LABEL_SHAPE_WIDGET_FEATURE_PREPARED_STAGE_IMPL_H
