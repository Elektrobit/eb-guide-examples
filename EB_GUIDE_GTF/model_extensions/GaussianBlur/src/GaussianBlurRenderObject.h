////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Elektrobit Automotive GmbH
// Alle Rechte vorbehalten. All Rights Reserved.
//
// Information contained herein is subject to change without notice.
// Elektrobit retains ownership and all other rights in the software and each
// component thereof.
// Any reproduction of the software or components thereof without the prior
// written permission of Elektrobit is prohibited.
///////////////////////////////////////////////////////////////////////////////

#ifndef GAUSSIAN_BLUR_RENDER_OBJECT_H
#define GAUSSIAN_BLUR_RENDER_OBJECT_H

#include <gtf/properties/Container.h>
#include <gtf/types/TypesC99.h>
#include <gtf/scdr/DataStore.h>
#include <gtf/scdr/command/CommandBuffer.h>
#include <gtf/scdr/RenderObjectBase.h>
#include <gtf/scdr/allocator/Allocator.h>
#include <gtf/scdr/touch/TouchObject.h>
#include "GaussianBlurStackFrame.h"

#define MAX_FILTER_WEIGHT_COUNT 32

struct GaussianBlurWeightParams
{
    uint32_t width;
    uint32_t height;
    float    sigma;
};


struct GaussianBlurUniforms
{
    gtf::utils::math::float4x4 projMat;
    gtf::utils::math::float4x4 modelMat;
    float                      sigma;
    float                      weights[MAX_FILTER_WEIGHT_COUNT];
    float                      offsetx[MAX_FILTER_WEIGHT_COUNT];
    float                      offsety[MAX_FILTER_WEIGHT_COUNT];
    uint32_t                   weightCount;
    bool                       enabled;
};

struct AlphaMaskUniforms
{
    gtf::utils::math::float4x4 object2Alpha;
};

class GaussianBlurRenderObject
    : public gtf::scdr::RenderObjectBase,
      public gtf::scdr::TouchObject
{
public:
    enum class Pass : uint32_t
    {
        Horizontal,
        Vertical,
        Count
    };

public:
    GaussianBlurRenderObject(gtf::properties::Container& model_, gtf::scdr::GraphicsApi api_, const gtf::scdr::WidgetFeaturesCommon2D& features_, const gtf::scdr::WidgetPropertiesCommon2D& properties_);

    gtf::scdr::RenderObjectStackFrame* createStackFrame(gtf::scdr::RenderObjectStackFrame& parent_, gtf::scdr::Allocator& allocator_) override;
    void                               destroyStackFrame(gtf::scdr::RenderObjectStackFrame* frame_, gtf::scdr::Allocator& allocator_) override;
    const gtf::scdr::TouchObject*      getTouchObject() override;

    void getBoundingRectangle(gtf::scdr::Rectangle<int16_t>& rectOut_) const final override;
    bool intersect(const gtf::scdr::ScreenPosition& pos_) const final override;

    bool updateFramebuffer(
        const gtf::scdr::Framebuffer* fbParent_,
        int32_t                       width_,
        int32_t                       height_,
        gtf::scdr::DataStore&         dataStore_,
        gtf::scdr::CommandBuffer&     commandBuffer_);

    void releaseFramebuffers(gtf::scdr::DataStore& dataStore_, gtf::scdr::CommandBuffer& commandBuffer_);
    bool acquireFramebuffers(const gtf::scdr::FramebufferDescriptor& fbdesc_, gtf::scdr::DataStore& dataStore_, gtf::scdr::CommandBuffer& commandBuffer_);

    void setWeights(const GaussianBlurWeightParams& params);
    void setDefaultWeights();
    void updateWeights(float sigma_);

public:
    GaussianBlurRenderObject*   next;
    gtf::scdr::GraphicsApi      api;
    gtf::scdr::Viewport         viewport;
    gtf::scdr::Framebuffer*     framebuffer;
    uint32_t                    framebufferId;
    uint32_t                    offscreenFramebufferId;
    uint32_t                    drawId;
    GaussianBlurUniforms        effectUniforms;
    AlphaMaskUniforms           alphaMaskUniforms;
    GaussianBlurWeightParams    currentWeightParams;
    const gtf::scdr::AlphaMask* alphaMask;
    gtf::scdr::Framebuffer*     fbParent;
    gtf::scdr::Framebuffer*     fbChildren;
    gtf::scdr::Framebuffer*     fbBlur[static_cast<uint32_t>(Pass::Count)];
};

#endif // GAUSSIAN_BLUR_RENDER_OBJECT_H
