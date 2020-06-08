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

#ifndef BEZIER_CURVE_DESCRIPTOR_PROVIDER_H
#define BEZIER_CURVE_DESCRIPTOR_PROVIDER_H

#include <gtf/metainformation/DescriptorProvider.h>
namespace  bezierCurve
{
    class BezierCurveDescriptorProvider : public gtf::metainformation::DescriptorProvider
    {
    public:
        virtual gtf::metainformation::WidgetDescriptor const* GetWidgets(uint32_t& count) const;

        virtual bool ResolveTypes(gtf::typesystem::TypeManager const * typeManager);

        virtual gtf::metainformation::PopupStackDescriptor const * GetPopupStacks(uint32_t & count) const
        {
            count = 0;
            return NULL;
        }

        virtual gtf::metainformation::ActionDescriptor const * GetActions(uint32_t& count) const
        {
            count = 0;
            return NULL;
        }

        virtual gtf::metainformation::WidgetFeatureDescriptor const * GetWidgetFeatures(uint32_t & count) const
        {
            count = 0;
            return NULL;
        }

        virtual gtf::metainformation::ResourceDescriptor const * GetResourceTypes(uint32_t & count) const
        {
            count = 0;
            return NULL;
        }

    };
}

#endif // BEZIER_CURVE_DESCRIPTOR_PROVIDER_H
