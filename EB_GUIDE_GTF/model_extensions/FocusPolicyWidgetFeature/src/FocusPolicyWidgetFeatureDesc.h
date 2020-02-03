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

#ifndef GTF_FOCUSED_POLICY_WIDGET_FEATURE_DESC_H_INCLUDED
#define GTF_FOCUSED_POLICY_WIDGET_FEATURE_DESC_H_INCLUDED

#include <gtf/metainformation/DescriptorProvider.h>
#include "FocusPolicyWidgetFeature.h"

namespace focusedpolicywidgetfeature {
class FocusedPolicyWidgetFeatureDesc : public gtf::metainformation::DescriptorProvider
{
public:
    virtual gtf::metainformation::ActionDescriptor const* GetActions(uint32_t& count_) const;

    virtual gtf::metainformation::PopupStackDescriptor const* GetPopupStacks(uint32_t& count_) const;

    virtual gtf::metainformation::WidgetDescriptor const* GetWidgets(uint32_t& count_) const;

    virtual gtf::metainformation::WidgetFeatureDescriptor const* GetWidgetFeatures(uint32_t& count_) const;

    virtual gtf::metainformation::ResourceDescriptor const* GetResourceTypes(uint32_t& count_) const;

    virtual bool ResolveTypes(const gtf::typesystem::TypeManager* typeManager_);
};
} // namespace focusedpolicywidgetfeature
#endif // GTF_FOCUSED_POLICY_WIDGET_FEATURE_DESC_H_INCLUDED
