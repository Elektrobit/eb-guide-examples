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

#include "SineLabelShape.h"

#include <gtf/properties/Features.h>
#include <gtf/scdr/RenderObjectUtils.h>

using namespace gtf;
namespace labelshapewidgetfeature {
    labelshapewidgetfeature::SineLabelShape::SineLabelShape(const scene::TypeResolverHandle& resolver_, const properties::ContainerHandle model_)
        : resolver(resolver_)
        , model(model_)
        , stepWidth(0.f)
    {
        scene::TypeResolver::FeaturePropertyReference fpr[] = 
        {
            {{SineLabelShapeFeatureName, properties::Features::Feature()}, {"enabled",types::limits::uint16_max}},
            {{SineLabelShapeFeatureName, properties::Features::Feature()}, {"amplitude", types::limits::uint16_max}},
            {{SineLabelShapeFeatureName, properties::Features::Feature()}, {"periodLength", types::limits::uint16_max}},
            {{SineLabelShapeFeatureName, properties::Features::Feature()}, {"horizontalOffset", types::limits::uint16_max}},
            {{SineLabelShapeFeatureName, properties::Features::Feature()}, {"verticalOffset", types::limits::uint16_max}},
            {{SineLabelShapeFeatureName, properties::Features::Feature()}, {"cycleCount", types::limits::uint16_max}}
        };

        resolver->resolveFeatureProperties(fpr, ARRAY_SIZE(fpr));

        sineCurveFeatures.featureId        = fpr[0].featureType.identifier.getValue();
        sineCurveFeatures.enabled          = fpr[0].property.identifier;
        sineCurveFeatures.amplitude        = fpr[1].property.identifier;
        sineCurveFeatures.periodLength     = fpr[2].property.identifier;
        sineCurveFeatures.horizontalOffset = fpr[3].property.identifier;
        sineCurveFeatures.verticalOffset   = fpr[4].property.identifier;
        sineCurveFeatures.cycleCount       = fpr[5].property.identifier;

        modelID = scdr::LoadFeature(*model, sineCurveFeatures);
    }

    labelshapewidgetfeature::SineLabelShape::~SineLabelShape()
    {

    }

    int32_t binarySearch(const float* array, const uint32_t size, const float value)
    {
        uint32_t min = 1;
        uint32_t max = size - 1;

        while (min <= max)
        {
            const uint32_t mid = (min + max) / 2;

            if (array[mid] >= value && value >= array[mid - 1])
            {
                return mid;
            }

            else if (value < array[mid - 1])
            {
                max = mid - 1;
            }

            else if (value > array[mid])
            {
                min = mid + 1;
            }
        }

        return -1;
    }



    float labelshapewidgetfeature::SineLabelShape::getPointOnTimeline(float offsetFromStart_, uint32_t& cycleCount_)
    {
        // Using a lookup table to get the corresponding point (t) on the timeline to make glyphs equidistant
        // along the sine curve. Only The length of one cycle of the sine wave is stored in the lookup table.
        cycleCount_ = 0;
        if (fabs(offsetFromStart_) > lookup[stepCount - 1])
        {
            cycleCount_ = int(fabs(offsetFromStart_) / lookup[stepCount - 1]);
            offsetFromStart_ = offsetFromStart_ > 0 ? offsetFromStart_ - cycleCount_ * lookup[stepCount - 1] : offsetFromStart_;
        }
        while (offsetFromStart_ < 0)
        {
            offsetFromStart_ += lookup[stepCount - 1];
        }

        const int32_t index = binarySearch(lookup, stepCount, offsetFromStart_);

        if (index > 0)
        {
            const float previousT = (index - 1) * stepWidth;
            const float nextT = index * stepWidth;
            const float previousY = lookup[index - 1];
            const float nextY = lookup[index];

            float t = previousT + ((offsetFromStart_ - previousY) / (nextY - previousY)) * (nextT - previousT);
            t += cycleCount_ * sineCurveFeatureValues.periodLength;
            return t;
        }

        return -1.f;
    }

    scdr::GlyphTransform labelshapewidgetfeature::SineLabelShape::getTransform(const float offsetFromStart_)
    {
        using utils::math::Pi;

        const float amplitude = sineCurveFeatureValues.amplitude;
        const float periodLength = sineCurveFeatureValues.periodLength;

        uint32_t textCycleCount = 0;
        const float t = getPointOnTimeline(offsetFromStart_, textCycleCount);
        if (t < 0.f)
        {
            return scdr::GlyphTransform{ 0, 0, 0};
        }

        float x = offsetFromStart_ > 0 ? t : -((textCycleCount + 1) * periodLength - t) - textCycleCount * periodLength;
        x      += sineCurveFeatureValues.horizontalOffset;

        const float angularFrequency = 2 * Pi / periodLength;
        const float y = -sinf(angularFrequency * t) * amplitude + sineCurveFeatureValues.verticalOffset;

        const float rot = atanf(amplitude * cosf(angularFrequency * t) * angularFrequency);

        return scdr::GlyphTransform{ x, y, rot};
    }

    void buildLookupTable(float* lookup_, const uint32_t stepCount_, const float periodLength_, const float amplitude_, float& stepWidth)
    {
        using utils::math::Pi;
        float accumulativeLength = 0;
        float previousY = 0.0f;
        lookup_[0] = 0.f;
        stepWidth = periodLength_ / stepCount_;
        for (uint32_t i = 1; i < stepCount_; ++i)
        {
            const float currentX = i * stepWidth;
            const float currentY = amplitude_ * sinf(2 * Pi * currentX / periodLength_);

            const float x = stepWidth;
            const float y = currentY - previousY;
            const float segmentLength = sqrtf(x * x + y * y);
            accumulativeLength += segmentLength;

            lookup_[i] = accumulativeLength;
            previousY = currentY;
        }
    }

    bool labelshapewidgetfeature::SineLabelShape::updateModel()
    {
        GTF_OSAL_ASSERT(model.valid());
        if (modelID)
        {
            bool widget_dirty = false;
            bool table_dirty = false;

            scdr::LoadFeatureValue(*model, sineCurveFeatures.featureId, sineCurveFeatures.enabled, sineCurveFeatureValues.enabled);

            const float amplitude = sineCurveFeatureValues.amplitude;
            scdr::LoadFeatureValue(*model, sineCurveFeatures.featureId, sineCurveFeatures.amplitude, sineCurveFeatureValues.amplitude);
            table_dirty  = (amplitude != sineCurveFeatureValues.amplitude) ? true : table_dirty;
            widget_dirty = (amplitude != sineCurveFeatureValues.amplitude) ? true : widget_dirty;

            const float periodLength = sineCurveFeatureValues.periodLength;
            scdr::LoadFeatureValue(*model, sineCurveFeatures.featureId, sineCurveFeatures.periodLength, sineCurveFeatureValues.periodLength);
            table_dirty  = (periodLength != sineCurveFeatureValues.periodLength) ? true : table_dirty;
            widget_dirty = (periodLength != sineCurveFeatureValues.periodLength) ? true : widget_dirty;

            const float horizontalOffset = sineCurveFeatureValues.horizontalOffset;
            scdr::LoadFeatureValue(*model, sineCurveFeatures.featureId, sineCurveFeatures.horizontalOffset, sineCurveFeatureValues.horizontalOffset);
            widget_dirty = (horizontalOffset != sineCurveFeatureValues.horizontalOffset) ? true : widget_dirty;

            const float verticalOffset = sineCurveFeatureValues.verticalOffset;
            scdr::LoadFeatureValue(*model, sineCurveFeatures.featureId, sineCurveFeatures.verticalOffset, sineCurveFeatureValues.verticalOffset);
            widget_dirty = (verticalOffset != sineCurveFeatureValues.verticalOffset) ? true : widget_dirty;

            const float cycleCount = sineCurveFeatureValues.cycleCount;
            scdr::LoadFeatureValue(*model, sineCurveFeatures.featureId, sineCurveFeatures.cycleCount, sineCurveFeatureValues.cycleCount);
            widget_dirty = (cycleCount != sineCurveFeatureValues.cycleCount) ? true : widget_dirty;

            // Rebuild lookup table if either amplitude or period length are changed.
            if (table_dirty)
            {
                buildLookupTable(lookup, stepCount, sineCurveFeatureValues.periodLength, sineCurveFeatureValues.amplitude, stepWidth);
            }

            return widget_dirty;
        }

        return false;
    }

    gtf::scdr::TruncationRange labelshapewidgetfeature::SineLabelShape::getTruncationRange()
    {
        const int32_t sineCurvelength = static_cast<int32_t>(lookup[stepCount - 1] * sineCurveFeatureValues.cycleCount);
        return scdr::TruncationRange{ 0, sineCurvelength };
    }

    bool labelshapewidgetfeature::SineLabelShape::enabled() const
    {
        return sineCurveFeatureValues.enabled;
    }
} // namespace labelshapewidgetfeature

