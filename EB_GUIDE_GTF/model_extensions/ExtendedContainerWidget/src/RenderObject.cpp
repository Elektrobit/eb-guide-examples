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

#include "RenderObject.h"
#include "RenderObjectStackFrame.h"

extendedcontainerwidget::RenderObject::RenderObject(gtf::properties::Container& widget_, const gtf::scdr::WidgetFeaturesCommon2D& featureHandles_, const gtf::scdr::WidgetPropertiesCommon2D& propertyHandles_)
{
    //
    // We need to map the feature handles to actual feature pointers
    //
    gtf::scdr::GetWidgetFeaturesCommon2DModels(widget_, featureHandles_, features.models);

    // Get widget feature property handles from the widget model
    gtf::scdr::GetWidgetFeaturesCommon2DHandles(widget_, featureHandles_, features.models, features.common2d);

    // Get the widget property handles from the widget model
    gtf::scdr::GetWidgetPropertiesCommon2DHandles(widget_, propertyHandles_, properties.common2d);
}

gtf::scdr::RenderObjectStackFrame* extendedcontainerwidget::RenderObject::createStackFrame(gtf::scdr::RenderObjectStackFrame&, gtf::scdr::Allocator& allocator_)
{
    return ALLOC_OBJECT(allocator_, RenderObjectStackFrame);
}

void extendedcontainerwidget::RenderObject::destroyStackFrame(gtf::scdr::RenderObjectStackFrame* frame_, gtf::scdr::Allocator& allocator_)
{
    FREE_OBJECT(allocator_, static_cast<RenderObjectStackFrame*>(frame_));
}

const gtf::scdr::TouchObject* extendedcontainerwidget::RenderObject::getTouchObject()
{
    if (features.models.touch && width > 0.f && height > 0.f)
    {
        return this;
    }
    return NULL;
}

bool extendedcontainerwidget::RenderObject::transformCoordinates(gtf::scdr::TransformMode::Value mode_, uint32_t coordCount_, gtf::scdr::TransformCoordinate* coordInOut_) const
{
    return gtf::scdr::CalculateCoordinateTransformation(widget2Screen, width, height, mode_, coordCount_, coordInOut_);
}

bool extendedcontainerwidget::RenderObject::getTransformationMatrix(gtf::scdr::TransformMode::Value mode_, gtf::utils::math::float4x4& transformationMatrix_) const
{
    return gtf::scdr::CalculateTransformationMatrix(widget2Screen, width, height, mode_, transformationMatrix_);
}

void extendedcontainerwidget::RenderObject::getBoundingRectangle(gtf::scdr::Rectangle<int16_t>& rectOut_) const
{
    gtf::scdr::TransformCoordinate coord[] =
        {
            { 0.f, 0.f },
            { 0.f, 1.f },
            { 1.f, 0.f },
            { 1.f, 1.f }
        };

    transformCoordinates(gtf::scdr::TransformMode::Unit2Screen, ARRAY_SIZE(coord), coord);

    rectOut_.xmin = static_cast<int16_t>(MIN(coord[0].x, MIN(coord[1].x, MIN(coord[2].x, coord[3].x))));
    rectOut_.xmax = static_cast<int16_t>(MAX(coord[0].x, MAX(coord[1].x, MAX(coord[2].x, coord[3].x))));
    rectOut_.ymin = static_cast<int16_t>(MIN(coord[0].y, MIN(coord[1].y, MIN(coord[2].y, coord[3].y))));
    rectOut_.ymax = static_cast<int16_t>(MAX(coord[0].y, MAX(coord[1].y, MAX(coord[2].y, coord[3].y))));
}

bool extendedcontainerwidget::RenderObject::intersect(const gtf::scdr::ScreenPosition& pos_) const
{
    gtf::scdr::TransformCoordinate coord =
        {
            static_cast<float>(pos_.x), static_cast<float>(pos_.y)
        };

    transformCoordinates(gtf::scdr::TransformMode::Screen2Widget, 1, &coord);

    return 0.f <= coord.x && coord.x <= width && 0.f <= coord.y && coord.y <= height;
}
