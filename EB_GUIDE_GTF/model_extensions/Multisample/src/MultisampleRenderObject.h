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

#ifndef MULTISAMPLE_RENDER_OBJECT_H
#define MULTISAMPLE_RENDER_OBJECT_H

#include <gtf/properties/Container.h>
#include <gtf/types/TypesC99.h>
#include <gtf/scdr/DataStore.h>
#include <gtf/scdr/command/CommandBuffer.h>
#include <gtf/scdr/RenderObjectBase.h>
#include <gtf/scdr/allocator/Allocator.h>
#include <gtf/scdr/touch/TouchObject.h>

#include "MultisampleStackFrame.h"

struct MultisampleUniforms
{
    struct
    {
        gtf::utils::math::float4x4 modelMat;
        gtf::utils::math::float4x4 projMat;
        gtf::scdr::Color           fillColor;
    } offscreenPass;

    struct
    {
        gtf::utils::math::float4x4 projMat;
        gtf::utils::math::float4x4 quadMat;
    } defaultPass;

    gtf::utils::math::float4x4 object2Alpha;
};

class MultisampleRenderObject
    : public gtf::scdr::RenderObjectBase,
    public gtf::scdr::TouchObject
{
public:
    MultisampleRenderObject(gtf::properties::Container& widget_, const gtf::scdr::WidgetFeaturesCommon2D& featureHandles_, const gtf::scdr::WidgetPropertiesCommon2D& propertyHandles_);

public: // sdk::RenderObject interface
    virtual gtf::scdr::RenderObjectStackFrame* createStackFrame(gtf::scdr::RenderObjectStackFrame&, gtf::scdr::Allocator& allocator_) override;

    virtual void destroyStackFrame(gtf::scdr::RenderObjectStackFrame* frame_, gtf::scdr::Allocator& allocator_) override;

    virtual const gtf::scdr::TouchObject* getTouchObject() override;

public: // sdk::TouchObject interface
    virtual void getBoundingRectangle(gtf::scdr::Rectangle<int16_t>& rectOut_) const override;

    virtual bool intersect(const gtf::scdr::ScreenPosition& pos_) const override;

public:
    MultisampleRenderObject*    next;
    gtf::scdr::Viewport         viewport;
    gtf::scdr::Framebuffer*     framebuffer;
    gtf::scdr::Framebuffer*     fbRender;
    gtf::scdr::Framebuffer*     fbResolve;
    uint32_t                    framebufferId;
    const gtf::scdr::AlphaMask* alphaMask;
    MultisampleUniforms         uniforms;
    uint32_t                    drawId;
};

#endif // MULTISAMPLE_RENDER_OBJECT_H
