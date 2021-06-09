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

#ifndef CUSTOM_WIDGET_BLOOMEFFECT_RENDERER_H_INCLUDED
#define CUSTOM_WIDGET_BLOOMEFFECT_RENDERER_H_INCLUDED

#include <gtf/scdr/RenderObjectUtils.h>
#include <gtf/scdr/widgets/IWidgetRenderer.h>
#include <gtf/scdr/gpu/GraphicsApi.h>
#include <gtf/scdr/gpu/ShaderConstant.h>
#include <gtf/scdr/gpu/VertexBuffer.h>

#define BLOOMEFFECT_PLUGIN_NAME "BloomEffect"

struct BloomEffectShader
{
    enum Value
    {
        Luminance           = 0,
        GaussianBlurVert    = 1,
        GaussianBlurHor     = 2,
        FinalBloom          = 3,
        FinalBloomWithAlpha = 4,
        Count
    };
};

struct BloomEffectShaderConstants
{
    gtf::scdr::ShaderConstant* object2Screen;
    gtf::scdr::ShaderConstant* object2Alpha;
    gtf::scdr::ShaderConstant* size;
    gtf::scdr::ShaderConstant* lightColor;
    gtf::scdr::ShaderConstant* threshold;
    gtf::scdr::ShaderConstant* luminanceBlurPath;
    gtf::scdr::ShaderConstant* colorTexture;
    gtf::scdr::ShaderConstant* offscreenTexture;
    gtf::scdr::ShaderConstant* bloomBlurTexture;
    gtf::scdr::ShaderConstant* alphaMaskTexture;
};

struct BloomEffectGraphicObjects
{
    BloomEffectGraphicObjects()
        : vb(NULL)
        , samplerAlphaMask(NULL)
        , driver(gtf::scdr::GraphicsApi::OpenGLES3)
    {
        memset(sampler, 0, sizeof(sampler));
        memset(pso, 0, sizeof(pso));
        memset(shaderConstants, 0, sizeof(shaderConstants));
    }

    gtf::scdr::VertexBuffer*        vb;
    gtf::scdr::PipelineStateObject* pso[BloomEffectShader::Count];
    BloomEffectShaderConstants      shaderConstants[BloomEffectShader::Count];
    gtf::scdr::SamplerState*        sampler[2];
    gtf::scdr::SamplerState*        samplerAlphaMask;
    gtf::scdr::GraphicsApi          driver;
};

class BloomEffectRenderObject;

struct BloomEffectProperties
{
    gtf::scdr::WidgetPropertiesCommon2D common2d;
    uint16_t                            luminosityRGB;
    uint16_t                            threshold;
    uint16_t                            luminanceBlurPath;
    uint16_t                            blurIterations;
};

struct BloomEffectFeatures
{
    gtf::scdr::WidgetFeaturesCommon2D common2d;
};

class BloomEffectRenderer : public gtf::scdr::IWidgetRenderer
{
private:
    BloomEffectRenderer(const BloomEffectRenderer&);
    BloomEffectRenderer& operator=(const BloomEffectRenderer&);

    BloomEffectRenderer(const gtf::scene::TypeResolver& resolver_, gtf::scdr::GraphicsApi graphicsApi_);

public:
    static gtf::scdr::IWidgetRenderer* createIntance(
        const gtf::scene::TypeResolver& resolver_,
        gtf::scdr::DataStore&           dataStore_,
        gtf::scdr::CommandBuffer&       commandBuffer_,
        gtf::scdr::Allocator&           frameAlloactor_,
        const void*                     createParams_);

    static void destroyInstance(
        gtf::scdr::IWidgetRenderer* renderer_,
        gtf::scdr::DataStore&       dataStore_,
        gtf::scdr::CommandBuffer&   commandBuffer_,
        gtf::scdr::Allocator&       frameAlloactor_);

    virtual ~BloomEffectRenderer();

public:
    virtual void                     deinit(gtf::scdr::DataStore& dataStore_, gtf::scdr::CommandBuffer& commandBuffer_, gtf::scdr::Allocator& frameAlloactor_);
    virtual bool                     init(const gtf::scene::TypeResolver& typeResolver_, gtf::scdr::DataStore& dataStore_, gtf::scdr::CommandBuffer& commandBuffer_, gtf::scdr::Allocator& frameAllocator_);
    virtual gtf::scdr::RenderObject* createRenderObject(gtf::properties::Container& widget_, gtf::scdr::DataStore& dataStore_, gtf::scdr::CommandBuffer& commandBuffer_, gtf::scdr::Allocator& frameAllocator_);
    virtual void                     destroyRenderObject(gtf::scdr::RenderObject* object_, gtf::scdr::DataStore& dataStore_, gtf::scdr::CommandBuffer& commandBuffer_, gtf::scdr::Allocator& frameAllocator_);
    virtual bool                     traverse(gtf::properties::Container& widget_, const gtf::scdr::RenderObjectStackFrame& parentStackFrame_, gtf::scdr::RenderObjectStackFrame& objectStackFrame_, gtf::scdr::RenderObject& object_, gtf::scdr::TraverserContext& traverserContext_);
    virtual bool                     update(gtf::scdr::ResourceLoader& resourceLoader_, gtf::scdr::DataStore& dataStore_, gtf::scdr::CommandBuffer& commandBuffer_, gtf::scdr::Allocator& frameAllocator_);
    virtual bool                     render(gtf::scdr::DataStore& dataStore_, gtf::scdr::CommandBuffer& commandBuffer_, gtf::scdr::Allocator& frameAllocator_);
    virtual bool                     begin(gtf::scdr::DataStore& dataStore_, gtf::scdr::CommandBuffer& commandBuffer_, gtf::scdr::Allocator& frameAllocator_);
    virtual bool                     end(gtf::scdr::DataStore& dataStore_, gtf::scdr::CommandBuffer& commandBuffer_, gtf::scdr::Allocator& frameAllocator_);

private:
    bool loadShaderParameters(
        gtf::properties::Container& widget_,
        BloomEffectRenderObject&    object_);

    BloomEffectRenderObject*  objects;
    BloomEffectGraphicObjects graphicObjects;
    BloomEffectProperties     properties;
    BloomEffectFeatures       features;
};

#endif // CUSTOM_WIDGET_BLOOMEFFECT_RENDERER_H_INCLUDED
