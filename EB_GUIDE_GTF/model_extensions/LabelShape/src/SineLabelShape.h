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

#ifndef GTF_SIN_LABEL_SHAPE_H_INCLUDED
#define GTF_SIN_LABEL_SHAPE_H_INCLUDED

#include <gtf/scdr/LabelShape.h>
#include <gtf/scene/TypeResolver.h>
#include <gtf/properties/ScalarValue.h>

using namespace gtf;

namespace labelshapewidgetfeature {
const char SineLabelShapeFeatureName[] = "SineLabelShape";

struct SineCurveFeature
{
    gtf::properties::BoolValueHandle  enabled;
    gtf::properties::FloatValueHandle amplitude;
    gtf::properties::FloatValueHandle periodLength;
    gtf::properties::FloatValueHandle horizontalOffset;
    gtf::properties::FloatValueHandle verticalOffset;
    gtf::properties::FloatValueHandle cycleCount;
};

struct SineCurveFeatureValues
{
    SineCurveFeatureValues()
        : enabled(false)
        , amplitude(0.0f)
        , periodLength(0.0f)
        , horizontalOffset(0.0f)
        , verticalOffset(0.0f)
        , cycleCount(1.0f)
    {
    }

    bool  enabled;
    float amplitude;
    float periodLength;
    float horizontalOffset;
    float verticalOffset;
    float cycleCount;
};
/*!
    \brief Implementation of sine curve for label shape.
    */
class SineLabelShape : public scdr::LabelShape
{
public:
    SineLabelShape(const scene::TypeResolverHandle& resolver_, const properties::ContainerHandle model_);

    virtual ~SineLabelShape();

    virtual scdr::GlyphTransform getTransform(const float offset_) override;

    virtual scdr::TruncationRange getTruncationRange() override;

    virtual bool enabled() const override;

    virtual bool updateModel() override;

private:
    const gtf::scene::TypeResolverHandle resolver;
    const properties::ContainerHandle    model;

    SineCurveFeature       sineCurveFeatures;
    SineCurveFeatureValues sineCurveFeatureValues;
    bool                   isAssigned;

    float getPointOnTimeline(float offsetFromStart_, uint32_t& cycleCount_);

    float                 stepWidth = 0.f;
    static const uint32_t stepCount = 1024;
    float                 lookup[stepCount];
};

typedef utils::smartptr::RefCountedPtr<SineLabelShape> SineLabelShapeHandle;
} // namespace labelshapewidgetfeature

#endif // GTF_SIN_LABEL_SHAPE_H_INCLUDED
