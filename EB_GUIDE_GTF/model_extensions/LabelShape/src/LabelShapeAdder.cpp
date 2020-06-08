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

#include "LabelShapeAdder.h"
#include "SineLabelShape.h"

#include <gtf/properties/ScalarValue.h>

namespace labelshapewidgetfeature {

    // This string has a special meaning and it should be written in this way, so the widget feature can be resolved correctly.
    static const char labelShapeFeatureName[] = "labelShape";

    SineCurveShapeAdder::SineCurveShapeAdder(const scene::TypeResolverHandle& resolver_)
    {
        resolver = resolver_;
        labelShapeFeatureReference.featureType.name = labelShapeFeatureName;
        labelShapeFeatureReference.property.name = labelShapeFeatureName;

        resolver_->resolveFeatureProperties(&labelShapeFeatureReference, 1);
    }

    void SineCurveShapeAdder::addToWidget(properties::Container& widget_)
    {
        properties::Properties& widgetProperties = widget_.getProperties();
        const uint32_t          propertyCount = widgetProperties.count();
        
        widgetProperties.create(properties::Property::ValueType::RefCountedPtr, properties::Property::ImplType::Native, propertyCount);
        widget_.getFeatures().add(labelShapeFeatureReference.featureType.identifier, propertyCount, 1);
        
        const properties::RefCountedPtrValueHandle& labelShapeFunction = widgetProperties.get<properties::RefCountedPtrValue>(labelShapeFeatureReference.featureType.identifier, labelShapeFeatureReference.property.identifier);
        
        labelShapeFunction->set(new SineLabelShape(resolver, &widget_));
    }

    void SineCurveShapeAdder::connect(properties::Container&)
    {
    }

    void SineCurveShapeAdder::disconnect()
    {
    }
} // namespace labelshapewidgetfeature


