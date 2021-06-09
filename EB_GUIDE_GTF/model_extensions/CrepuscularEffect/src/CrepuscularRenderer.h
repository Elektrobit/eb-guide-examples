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

#ifndef CUSTOM_WIDGET_CREPUSCULAR_RENDERER_H_INCLUDED
#define CUSTOM_WIDGET_CREPUSCULAR_RENDERER_H_INCLUDED

#include <gtf/scdr/RenderObjectUtils.h>
#include <gtf/scdr/gpu/GraphicsApi.h>
#include <gtf/scdr/gpu/ShaderConstant.h>
#include <gtf/scdr/gpu/VertexBuffer.h>
#include <gtf/scdr/widgets/IWidgetRenderer.h>

#ifndef CREPUSCULAR_PLUGIN_NAME
#define CREPUSCULAR_PLUGIN_NAME "CrepuscularEffect"
#endif

struct CrepuscularShaderConstants
{
    gtf::scdr::ShaderConstant* object2Screen;
    gtf::scdr::ShaderConstant* color;
    gtf::scdr::ShaderConstant* pos;
    gtf::scdr::ShaderConstant* decay;
    gtf::scdr::ShaderConstant* density;
    gtf::scdr::ShaderConstant* weight;
    gtf::scdr::ShaderConstant* samples;
    gtf::scdr::ShaderConstant* object2Alpha;
    gtf::scdr::ShaderConstant* colorTexture;
    gtf::scdr::ShaderConstant* alphaMaskTexture;
};

struct CrepuscularGraphicObjects
{
    gtf::scdr::VertexBuffer*        vb;
    gtf::scdr::PipelineStateObject* pso;
    gtf::scdr::PipelineStateObject* psoAlphaMask;
    CrepuscularShaderConstants      shaderConstants;
    CrepuscularShaderConstants      shaderConstantsAlphaMask;
    gtf::scdr::SamplerState*        colorSampler;
    gtf::scdr::SamplerState*        alphaMaskSampler;
    gtf::scdr::GraphicsApi          driver;
};

class CrepuscularRenderObject;

struct CrepuscularConstantBufferBlock
{
    CrepuscularConstantBufferBlock()
        : color(gtf::scdr::SystemColors::Null)
        , decay(0.f)
        , density(0.f)
        , weight(0.f)
        , samples(0)
    {
        memset(pos, 0, sizeof(pos));
        memset(object2Screen, 0, sizeof(object2Screen));
    }

    float            object2Screen[16];
    gtf::scdr::Color color;

    float pos[2];

    float decay;
    float density;
    float weight;
    int   samples;
};

struct AlphaMaskConstantBufferBlock
{
    AlphaMaskConstantBufferBlock()
    {
        memset(object2Alpha, 0, sizeof(object2Alpha));
    }

    float object2Alpha[16];
};

struct CrepuscularProperties
{
    gtf::scdr::WidgetPropertiesCommon2D common2d;
    uint16_t                            color;
    uint16_t                            xpos;
    uint16_t                            ypos;
    uint16_t                            samples;
    uint16_t                            decay;
    uint16_t                            density;
    uint16_t                            weight;
};

struct CrepuscularFeatures
{
    gtf::scdr::WidgetFeaturesCommon2D common2d;
};

class CrepuscularRenderer : public gtf::scdr::IWidgetRenderer
{
private:
    CrepuscularRenderer(const CrepuscularRenderer&);
    CrepuscularRenderer& operator=(const CrepuscularRenderer&);

    CrepuscularRenderer(const gtf::scene::TypeResolver& resolver_, gtf::scdr::GraphicsApi graphicsApi_);

public:
    static gtf::scdr::IWidgetRenderer* createInstance(
        const gtf::scene::TypeResolver& resolver_,
        gtf::scdr::DataStore&           dataStore_,
        gtf::scdr::CommandBuffer&       commandBuffer_,
        gtf::scdr::Allocator&           frameAllocator_,
        const void*                     createParams_);

    static void destroyInstance(
        gtf::scdr::IWidgetRenderer* renderer_,
        gtf::scdr::DataStore&       dataStore_,
        gtf::scdr::CommandBuffer&   commandBuffer_,
        gtf::scdr::Allocator&       frameAllocator_);

    virtual ~CrepuscularRenderer();

    virtual void                     deinit(gtf::scdr::DataStore& dataStore_, gtf::scdr::CommandBuffer& commandBuffer_, gtf::scdr::Allocator& frameAlloactor_);
    virtual bool                     init(const gtf::scene::TypeResolver& typeResolver_, gtf::scdr::DataStore& dataStore_, gtf::scdr::CommandBuffer& commandBuffer_, gtf::scdr::Allocator& frameAllocator_);
    virtual gtf::scdr::RenderObject* createRenderObject(gtf::properties::Container& widget_, gtf::scdr::DataStore& dataStore_, gtf::scdr::CommandBuffer& commandBuffer_, gtf::scdr::Allocator& frameAllocator_);
    virtual void                     destroyRenderObject(gtf::scdr::RenderObject* object_, gtf::scdr::DataStore& dataStore_, gtf::scdr::CommandBuffer& commandBuffer_, gtf::scdr::Allocator& frameAllocator_);
    virtual bool                     traverse(gtf::properties::Container& widget_, const gtf::scdr::RenderObjectStackFrame& parentStackFrame_, gtf::scdr::RenderObjectStackFrame& objectStackFrame_, gtf::scdr::RenderObject& object_, gtf::scdr::TraverserContext& traverserContext_);
    virtual bool                     update(gtf::scdr::ResourceLoader& resourceLoader_, gtf::scdr::DataStore& dataStore_, gtf::scdr::CommandBuffer& commandBuffer_, gtf::scdr::Allocator& frameAllocator_);
    virtual bool                     render(gtf::scdr::DataStore& dataStore_, gtf::scdr::CommandBuffer& commandBuffer_, gtf::scdr::Allocator& frameAllocator_);
    virtual bool                     begin(gtf::scdr::DataStore& dataStore_, gtf::scdr::CommandBuffer& commandBuffer_, gtf::scdr::Allocator& frameAllocator_);
    virtual bool                     end(gtf::scdr::DataStore& dataStore_, gtf::scdr::CommandBuffer& commandBuffer_, gtf::scdr::Allocator& frameAllocator_);

    CrepuscularRenderObject*  objects;
    CrepuscularGraphicObjects graphicObjects;
    CrepuscularProperties     properties;
    CrepuscularFeatures       features;
};

#endif // CUSTOM_WIDGET_CREPUSCULAR_RENDERER_H_INCLUDED
