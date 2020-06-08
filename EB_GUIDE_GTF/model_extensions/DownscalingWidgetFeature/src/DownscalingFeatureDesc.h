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

#ifndef DOWNSCALING_WIDGET_FEATURE_DESC_H
#define DOWNSCALING_WIDGET_FEATURE_DESC_H

#include <gtf/metainformation/DescriptorProvider.h>

namespace downscalingFeature
{

    class DownscalingFeatureDesc : public gtf::metainformation::DescriptorProvider
    {
    public:

        virtual gtf::metainformation::ActionDescriptor const* GetActions(uint32_t& count_) const;

        virtual gtf::metainformation::PopupStackDescriptor const* GetPopupStacks(uint32_t& count_) const;

        virtual gtf::metainformation::WidgetDescriptor const* GetWidgets(uint32_t& count_) const;

        virtual gtf::metainformation::WidgetFeatureDescriptor const* GetWidgetFeatures(uint32_t& count_) const;

        virtual gtf::metainformation::ResourceDescriptor const* GetResourceTypes(uint32_t& count_) const;

        virtual bool ResolveTypes(const gtf::typesystem::TypeManager* typeManager);

    };
}
#endif // DOWNSCALING_WIDGET_FEATURE_DESC_H
