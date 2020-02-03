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

#ifndef GTF_FOCUSED_WIDGET_FEATURE_H_INCLUDED
#define GTF_FOCUSED_WIDGET_FEATURE_H_INCLUDED

#include <gtf/scene/element/Controller.h>
#include <gtf/scene/TypeResolver.h>

#include <gtf/properties/PropertyObserver.h>
#include <gtf/properties/ScalarValue.h>

namespace focusedwidgetfeature {
class FocusRectColorFeature : public gtf::scene::element::Controller
{
public:
    FocusRectColorFeature(const gtf::scene::TypeResolverHandle& resolver_, const gtf::properties::PropertyObserverHandle& observer_);

    ~FocusRectColorFeature();

    virtual void connect(gtf::properties::Container& model_);

    virtual void disconnect();

protected:
    bool handleUpdate(const gtf::properties::PropertyKey& key);

private:
    gtf::properties::ContainerHandle        container;
    gtf::properties::PropertyObserverHandle observer;
    gtf::properties::BoolValueHandle        focusedProperty;
    gtf::properties::ColorValueHandle       focusedColorProperty;
    gtf::properties::ColorValueHandle       fillColorProperty;

    uint32_t initialColor;

    struct FeaturePropertyReferences
    {
        enum
        {
            FeaturePropFocusedColor = 0,
            Focused,
            FeaturePropMAX
        };
    };

    gtf::scene::TypeResolver::FeaturePropertyReference featurePropertyReferences[FeaturePropertyReferences::FeaturePropMAX];

    struct PropertyReferences
    {
        enum
        {
            FillColor,
            WidgetPropMAX
        };
    };

    gtf::scene::TypeResolver::ContainerPropertyReference widgetPropertyReferences[PropertyReferences::WidgetPropMAX];
};
} // namespace focusedwidgetfeature
GTF_DEFINE_INTERFACE_NAME(focusedwidgetfeature::FocusRectColorFeature)
#endif // GTF_FOCUSED_WIDGET_FEATURE_H_INCLUDED
