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

#include "FocusedWidgetFeature.h"

focusedwidgetfeature::FocusRectColorFeature::FocusRectColorFeature(const gtf::scene::TypeResolverHandle& resolver_, const gtf::properties::PropertyObserverHandle& observer_)
    : observer(observer_)
    , initialColor(gtf::types::limits::uint32_max)
{
    // Define the property references that are used in the implementation.  The
    // feature uses a property from another feature, namely property focused of
    // StateFocused.
    featurePropertyReferences[FeaturePropertyReferences::Focused].featureType.name = "StateFocused";
    featurePropertyReferences[FeaturePropertyReferences::Focused].property.name    = "focused";

    // the feature uses the fillColor property of the rectangle widget
    widgetPropertyReferences[PropertyReferences::FillColor].containerType.name = "GtfRectangle";
    widgetPropertyReferences[PropertyReferences::FillColor].property.name      = "fillColor";

    featurePropertyReferences[FeaturePropertyReferences::FeaturePropFocusedColor].featureType.name =
        "GtfFocusRectColorFeature";
    featurePropertyReferences[FeaturePropertyReferences::FeaturePropFocusedColor].property.name =
        "focusedColor";

    resolver_->resolveContainerProperties(widgetPropertyReferences, PropertyReferences::WidgetPropMAX);
    resolver_->resolveFeatureProperties(featurePropertyReferences, FeaturePropertyReferences::FeaturePropMAX);
}

void focusedwidgetfeature::FocusRectColorFeature::connect(gtf::properties::Container& model_)
{
    container = &model_;

    gtf::properties::Properties& widgetProperties = container->getProperties();

    fillColorProperty = widgetProperties.get<gtf::properties::ColorValue>(widgetPropertyReferences[PropertyReferences::FillColor].property.identifier);
    if (!fillColorProperty)
    {
        return;
    }

    initialColor = fillColorProperty->get();

    focusedProperty = widgetProperties.get<gtf::properties::BoolValue>(featurePropertyReferences[FeaturePropertyReferences::Focused].featureType.identifier, featurePropertyReferences[FeaturePropertyReferences::Focused].property.identifier);
    if (!focusedProperty.valid())
    {
        return;
    }

    focusedColorProperty = widgetProperties.get<gtf::properties::ColorValue>(featurePropertyReferences[FeaturePropertyReferences::FeaturePropFocusedColor].featureType.identifier, featurePropertyReferences[FeaturePropertyReferences::FeaturePropFocusedColor].property.identifier);
    if (!focusedColorProperty.valid())
    {
        return;
    }

    gtf::properties::PropertyKey focusedPropertyKey = gtf::properties::PropertyKey(container, featurePropertyReferences[FeaturePropertyReferences::Focused].featureType.identifier, featurePropertyReferences[FeaturePropertyReferences::Focused].property.identifier);

    // subscribe to changes of the focus property
    observer->subscribe(focusedPropertyKey, this, gtf::properties::PropertyObserver::Callback::create<FocusRectColorFeature, &FocusRectColorFeature::handleUpdate>(this));

    handleUpdate(focusedPropertyKey);
}

void focusedwidgetfeature::FocusRectColorFeature::disconnect()
{
    gtf::properties::PropertyKey focusedPropertyKey = gtf::properties::PropertyKey(container, featurePropertyReferences[FeaturePropertyReferences::Focused].featureType.identifier, featurePropertyReferences[FeaturePropertyReferences::Focused].property.identifier);

    observer->unsubscribe(focusedPropertyKey, this);
    container.reset();
    focusedProperty.reset();
    fillColorProperty.reset();
    focusedColorProperty.reset();
}

bool focusedwidgetfeature::FocusRectColorFeature::handleUpdate(const gtf::properties::PropertyKey&)
{
    // update fill color value, depending on the focused flag
    uint32_t newFillColor = initialColor;

    bool isFocused = focusedProperty->get();
    if (isFocused)
    {
        newFillColor = focusedColorProperty->get();
    }

    fillColorProperty->set(newFillColor);

    return true;
}

focusedwidgetfeature::FocusRectColorFeature::~FocusRectColorFeature()
{
}
