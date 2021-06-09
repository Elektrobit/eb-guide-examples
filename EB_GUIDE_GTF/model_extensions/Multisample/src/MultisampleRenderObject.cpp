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

#include "MultisampleRenderObject.h"

MultisampleRenderObject::MultisampleRenderObject(gtf::properties::Container& widget_, const gtf::scdr::WidgetFeaturesCommon2D& featureHandles_, const gtf::scdr::WidgetPropertiesCommon2D& propertyHandles_)
    : RenderObjectBase(widget_, featureHandles_, propertyHandles_)
    , next(nullptr)
    , framebuffer(nullptr)
    , fbRender(nullptr)
    , fbResolve(nullptr)
    , framebufferId(0)
    , alphaMask(nullptr)
    , drawId(0)
{
    memset(&uniforms, 0, sizeof(MultisampleUniforms));
}

gtf::scdr::RenderObjectStackFrame* MultisampleRenderObject::createStackFrame(gtf::scdr::RenderObjectStackFrame&, gtf::scdr::Allocator& allocator_)
{
    return ALLOC_OBJECT(allocator_, MultisampleStackFrame);
}

void MultisampleRenderObject::destroyStackFrame(gtf::scdr::RenderObjectStackFrame* frame_, gtf::scdr::Allocator& allocator_)
{
    FREE_OBJECT(allocator_, static_cast<MultisampleStackFrame*>(frame_));
}

const gtf::scdr::TouchObject* MultisampleRenderObject::getTouchObject()
{
    if (featureValues.mask & gtf::scdr::WidgetFeaturesCommon2DValues::Touch && width > 0.f && height > 0.f)
    {
        return this;
    }
    return NULL;
}

void MultisampleRenderObject::getBoundingRectangle(gtf::scdr::Rectangle<int16_t>& rectOut_) const
{
    gtf::scdr::CalculateBoundingRectangle(*this, rectOut_);
}

bool MultisampleRenderObject::intersect(const gtf::scdr::ScreenPosition& pos_) const
{
    return gtf::scdr::IntersectsBoundingRectangle(*this, pos_);
}
