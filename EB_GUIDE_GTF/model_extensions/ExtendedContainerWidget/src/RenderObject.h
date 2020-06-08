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

#ifndef GTF_EXTENDED_CONTAINER_RENDER_OBJECT_H_INCLUDED
#define GTF_EXTENDED_CONTAINER_RENDER_OBJECT_H_INCLUDED

#include <gtf/scdr/SDK.h>
#include <gtf/scdr/RenderObjectUtils.h>

namespace extendedcontainerwidget {
class RenderObject
    : public gtf::scdr::RenderObject,
      public gtf::scdr::TouchObject
{
public:
    RenderObject(const gtf::properties::Container& widget_, const gtf::scdr::WidgetFeaturesCommon2D& featureHandles_);

public: // sdk::RenderObject interface
    virtual gtf::scdr::RenderObjectStackFrame* createStackFrame(gtf::scdr::RenderObjectStackFrame&, gtf::scdr::Allocator& allocator_);
    virtual void                               destroyStackFrame(gtf::scdr::RenderObjectStackFrame* frame_, gtf::scdr::Allocator& allocator_);
    virtual const gtf::scdr::TouchObject*      getTouchObject();
    virtual bool                               transformCoordinates(gtf::scdr::TransformMode::Value mode_, uint32_t coordCount_, gtf::scdr::TransformCoordinate* coordInOut_) const;
    virtual bool                               getTransformationMatrix(gtf::scdr::TransformMode::Value mode_, gtf::utils::math::float4x4& transformationMatrix_) const;

public: // sdk::TouchObject interface
    virtual void getBoundingRectangle(gtf::scdr::Rectangle<int16_t>& rectOut_) const;
    virtual bool intersect(const gtf::scdr::ScreenPosition& pos_) const;

public:
    gtf::scdr::WidgetFeaturesCommon2DModels features;
    gtf::utils::math::float4x4              widget2Screen;
    float                                   width;
    float                                   height;
    gtf::scdr::Scissor                      scissor;
    uint32_t                                drawId;
};
} // namespace extendedcontainerwidget

#endif // GTF_EXTENDED_CONTAINER_RENDER_OBJECT_H_INCLUDED
