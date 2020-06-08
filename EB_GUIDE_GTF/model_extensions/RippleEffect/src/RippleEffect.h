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

#ifndef CUSTOM_WIDGET_RIPPLE_EFFECT_H_INCLUDED
#define CUSTOM_WIDGET_RIPPLE_EFFECT_H_INCLUDED

#include <gtf/scdr/RenderObjectUtils.h>
#include <gtf/scdr/gpu/Framebuffer.h>
#include <gtf/scdr/gpu/SamplerState.h>
#include <gtf/scdr/gpu/ShaderConstant.h>
#include <gtf/scdr/widgets/IWidgetRenderer.h>
#include <gtf/utils/math/matrix4x4.h>

class RippleEffectRenderObject;

struct RippleEffectProperties
{
    gtf::scdr::WidgetPropertiesCommon2D common2d;
    uint16_t                            enabled;
    uint16_t                            animationTime;
};

struct RippleEffectFeatures
{
    gtf::scdr::WidgetFeaturesCommon2D common2d;
};

struct RippleEffectShaderConstants
{
    gtf::scdr::ShaderConstant* projMat;
    gtf::scdr::ShaderConstant* modelMat;
    gtf::scdr::ShaderConstant* rippleTime;
    gtf::scdr::ShaderConstant* rippleEnable;
    gtf::scdr::ShaderConstant* object2Alpha;
    gtf::scdr::ShaderConstant* texture0;
    gtf::scdr::ShaderConstant* texture1;
};

struct RippleEffectUniforms
{
    gtf::utils::math::float4x4 projMat;
    gtf::utils::math::float4x4 modelMat;
    float               rippleTime;
    float               rippleEnable;
};

struct AlphaMaskUniforms
{
    gtf::utils::math::float4x4 object2Alpha;
};

struct RippleEffectGraphicObjects
{
    RippleEffectShaderConstants     shaderConstants;
    RippleEffectShaderConstants     shaderConstantsWithAlphaMask;
    gtf::scdr::PipelineStateObject* psoEffect;
    gtf::scdr::PipelineStateObject* psoWithAlphaMask;
    gtf::scdr::VertexBuffer*        vertices;
    gtf::scdr::SamplerState*        sampler0;
    gtf::scdr::SamplerState*        sampler1;
};

//!
//! \brief Implements an effect that distorts an offscreen framebuffer with a wave pattern.
//!
class RippleEffect : public gtf::scdr::IWidgetRenderer
{
private:
    RippleEffect(const RippleEffect&);
    RippleEffect& operator=(const RippleEffect&);

    RippleEffect(const gtf::scene::TypeResolver& resolver_);

public:
    //!
    //! \brief Creates an instance of the RippleEffect renderer.
    //!
    //! \param[in] context_ The SCDR graphics context.
    //! \param[in] createParams_ A pointer to user specific parameters.
    //!
    //! \return An instance of the custom renderer.
    //!
    //! \note
    //!   The lifetime of the createParams_ must extend over the entire lifetime of the program. Also, the
    //!   instance will be destroyed through destroyInstance on shutdown.
    //!
    static gtf::scdr::IWidgetRenderer* createInstance(
        const gtf::scene::TypeResolver& resolver_,
        gtf::scdr::DataStore&           dataStore_,
        gtf::scdr::CommandBuffer&       commandBuffer_,
        gtf::scdr::Allocator&           frameAllocator_,
        const void*                     createParams_);

    //!
    //! \brief Destroys an instance of the RippleEffect
    //!
    //! \param[in] renderer_ The renderer to destroy.
    //! \param[in] context_ The SCDR graphics context.
    //!
    //!
    static void destroyInstance(
        gtf::scdr::IWidgetRenderer* renderer_,
        gtf::scdr::DataStore&       dataStore_,
        gtf::scdr::CommandBuffer&   commandBuffer_,
        gtf::scdr::Allocator&       frameAllocator_);

    virtual ~RippleEffect();

public: // gtf::scdr::WidgetRenderer interface
    virtual bool init(const gtf::scene::TypeResolver& typeResolver_, gtf::scdr::DataStore& dataStore_, gtf::scdr::CommandBuffer& commandBuffer_, gtf::scdr::Allocator& frameAllocator_);
    virtual void deinit(gtf::scdr::DataStore& dataStore_, gtf::scdr::CommandBuffer& commandBuffer_, gtf::scdr::Allocator& frameAllocator_);

    virtual bool update(gtf::scdr::ResourceLoader& resourceLoader_, gtf::scdr::DataStore& dataStore_, gtf::scdr::CommandBuffer& commandBuffer_, gtf::scdr::Allocator& frameAllocator_);

    virtual bool render(gtf::scdr::DataStore& dataStore_, gtf::scdr::CommandBuffer& commandBuffer_, gtf::scdr::Allocator& frameAllocator_);

    virtual bool begin(gtf::scdr::DataStore& dataStore_, gtf::scdr::CommandBuffer& commandBuffer_, gtf::scdr::Allocator& frameAllocator_);
    virtual bool end(gtf::scdr::DataStore& dataStore_, gtf::scdr::CommandBuffer& commandBuffer_, gtf::scdr::Allocator& frameAllocator_);

    virtual gtf::scdr::RenderObject* createRenderObject(
        gtf::properties::Container& widget_,
        gtf::scdr::DataStore&       dataStore_,
        gtf::scdr::CommandBuffer&   commandBuffer_,
        gtf::scdr::Allocator&       frameAllocator_);

    virtual void destroyRenderObject(
        gtf::scdr::RenderObject*  object_,
        gtf::scdr::DataStore&     dataStore_,
        gtf::scdr::CommandBuffer& commandBuffer_,
        gtf::scdr::Allocator&     frameAllocator_);

    virtual bool traverse(
        gtf::properties::Container&              widget_,
        const gtf::scdr::RenderObjectStackFrame& parentStackFrame_,
        gtf::scdr::RenderObjectStackFrame&       objectStackFrame_,
        gtf::scdr::RenderObject&                 object_,
        gtf::scdr::TraverserContext&             traverserContext_);

private:
    static bool createOrUpdateFramebuffer(
        int32_t                       width_,
        int32_t                       height_,
        const gtf::scdr::Framebuffer* systemFramebuffer_,
        gtf::scdr::Framebuffer*&      fbInOut_,
        gtf::scdr::DataStore&         dataStore_,
        gtf::scdr::CommandBuffer&     commandBuffer_);

private:
    // A block of graphics objects that are common for all instances of the RippleEffectRenderObject
    RippleEffectGraphicObjects graphicObjects;

    // A linked list of all RippleEffectRenderObjects to draw
    RippleEffectRenderObject* objects;
    RippleEffectProperties    properties;
    RippleEffectFeatures      features;
};

#endif // CUSTOM_WIDGET_RIPPLE_EFFECT_H_INCLUDED
