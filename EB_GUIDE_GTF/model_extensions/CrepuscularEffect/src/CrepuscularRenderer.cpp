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

#include <gtf/scdr/allocator/Allocator.h>
#include <gtf/scdr/SDK.h>
#include <gtf/scdr/RenderObjectBase.h>
#include <gtf/scdr/gpu/Framebuffer.h>
#include <gtf/scdr/gpu/FramebufferClearFlags.h>
#include <gtf/scdr/gpu/GPUExecutionContext.h>

#include "CrepuscularRenderer.h"
#include "OpenGLES3Shader.h"

class CrepuscularStackFrame : public gtf::scdr::RenderObjectStackFrame2D
{
public:
    CrepuscularStackFrame()
        : framebufferId(0)
        , framebuffer(NULL)
        , widget2AlphaMask(gtf::utils::math::float4x4::Identity())
        , depth(0)
    {
        depthPtr = &depth;
    }

    virtual void load(gtf::scdr::RenderObjectStackFrame2DValues& valuesOut_) const
    {
        valuesOut_.coloration         = &coloration;
        valuesOut_.framebufferId      = framebufferId;
        valuesOut_.framebuffer        = framebuffer;
        valuesOut_.framebuffer2Screen = &framebuffer2Screen;
        valuesOut_.scissor            = &scissor;
        valuesOut_.viewport           = &viewport;
        valuesOut_.viewProjMat        = &viewProjMat;
        valuesOut_.widget2Framebuffer = &widget2Framebuffer;
        valuesOut_.depth              = depthPtr;
        valuesOut_.alphaMask          = NULL;
        valuesOut_.widget2AlphaMask   = &widget2AlphaMask;
    }

public:
    uint32_t                   framebufferId;
    gtf::scdr::Framebuffer*    framebuffer;
    gtf::utils::math::float4x4 framebuffer2Screen;
    gtf::utils::math::float4x4 viewProjMat;
    gtf::utils::math::float4x4 widget2Framebuffer;
    gtf::utils::math::float4x4 widget2AlphaMask;
    gtf::scdr::Color           coloration;
    gtf::scdr::Viewport        viewport;
    gtf::scdr::Scissor         scissor;
    uint32_t                   depth;

private:
    uint32_t* depthPtr;
};

class CrepuscularRenderObject
    : public gtf::scdr::RenderObjectBase,
      public gtf::scdr::TouchObject
{
private:
    CrepuscularRenderObject(const CrepuscularRenderObject&);
    CrepuscularRenderObject& operator=(const CrepuscularRenderObject&);

public:
    CrepuscularRenderObject(const CrepuscularGraphicObjects& graphicObjects_, gtf::properties::Container& widget_, const gtf::scdr::WidgetFeaturesCommon2D& features_, const gtf::scdr::WidgetPropertiesCommon2D& properties_)
        : RenderObjectBase(widget_, features_, properties_)
        , next(NULL)
        , graphicObjects(graphicObjects_)
        , framebuffer(NULL)
        , offscreenFramebuffer(NULL)
        , viewport(gtf::scdr::CViewport(0, 0, 0, 0))
        , alphaMask(NULL)
        , zid(0)
        , fbid(0)
        , offscreenFbId(0)
    {
    }

public: // RenderObject interface
    virtual gtf::scdr::RenderObjectStackFrame* createStackFrame(gtf::scdr::RenderObjectStackFrame& parent_, gtf::scdr::Allocator& allocator_)
    {
        GTF_UNUSED_PARAM(parent_);

        return ALLOC_OBJECT(allocator_, CrepuscularStackFrame);
    }

    virtual void destroyStackFrame(gtf::scdr::RenderObjectStackFrame* frame_, gtf::scdr::Allocator& allocator_)
    {
        FREE_OBJECT(allocator_, static_cast<CrepuscularStackFrame*>(frame_));
    }

    virtual const gtf::scdr::TouchObject* getTouchObject()
    {
        if (featureValues.mask & gtf::scdr::WidgetFeaturesCommon2DValues::Touch)
        {
            return this;
        }
        return NULL;
    }

public: // TouchObject interface
    virtual void getBoundingRectangle(gtf::scdr::Rectangle<int16_t>& rectOut_) const
    {
        gtf::scdr::CalculateBoundingRectangle(*this, rectOut_);
    }

    virtual bool intersect(const gtf::scdr::ScreenPosition& pos_) const
    {
        return gtf::scdr::IntersectsBoundingRectangle(*this, pos_);
    }

public:
    CrepuscularRenderObject* next;

    const CrepuscularGraphicObjects& graphicObjects;

    CrepuscularConstantBufferBlock effectConstantBufferBlock;
    AlphaMaskConstantBufferBlock   alphaMaskConstantBufferBlock;
    gtf::scdr::Framebuffer*        framebuffer;
    gtf::scdr::Framebuffer*        offscreenFramebuffer;
    gtf::scdr::Viewport            viewport;
    const gtf::scdr::AlphaMask*    alphaMask;

    uint32_t zid;
    uint32_t fbid;
    uint32_t offscreenFbId;
};

class ClearOffscreenFramebufferCommand : public gtf::scdr::Command
{
public:
    static ClearOffscreenFramebufferCommand* create(gtf::scdr::Framebuffer* fb_, gtf::scdr::CommandBuffer& commandBuffer_)
    {
        gtf::scdr::Allocator& commandAllocator = commandBuffer_.getCommandAllocator();

        return ALLOC_OBJECT(commandAllocator, ClearOffscreenFramebufferCommand, fb_);
    }

    ClearOffscreenFramebufferCommand(gtf::scdr::Framebuffer* fb_)
        : fb(fb_)
    {
    }

public:
    uint32_t execute(gtf::scdr::GPUExecutionContext& executionContext_, gtf::scdr::Allocator&)
    {
        static const uint32_t clearFlags =
            gtf::scdr::FramebufferClearFlags::Color | gtf::scdr::FramebufferClearFlags::Depth | gtf::scdr::FramebufferClearFlags::Stencil;

        executionContext_.clearFramebuffer(fb, clearFlags, gtf::scdr::SystemColors::Null, 1.0, 0);

        return gtf::scdr::CommandErrorCode::Success;
    }

private:
    gtf::scdr::Framebuffer* fb;
};

class DrawEffectCommand : public gtf::scdr::Command
{
public:
    static DrawEffectCommand* create(CrepuscularRenderObject* renderObject_, gtf::scdr::CommandBuffer& commandBuffer_)
    {
        gtf::scdr::Allocator& commandAllocator = commandBuffer_.getCommandAllocator();

        return ALLOC_OBJECT(commandAllocator, DrawEffectCommand, renderObject_);
    }

    DrawEffectCommand(CrepuscularRenderObject* renderObject_)
        : renderObject(renderObject_)
    {
    }

public:
    uint32_t execute(gtf::scdr::GPUExecutionContext& executionContext_, gtf::scdr::Allocator&)
    {
        if (renderObject->framebuffer)
        {
            const CrepuscularGraphicObjects& graphicObjects = renderObject->graphicObjects;

            gtf::scdr::Texture2D* const texture = renderObject->offscreenFramebuffer->getColorAttachment();

            const CrepuscularShaderConstants& shaderConstants = (renderObject->alphaMask != NULL)
                                                                    ? graphicObjects.shaderConstantsAlphaMask
                                                                    : graphicObjects.shaderConstants;

            // clang-format off
            const gtf::scdr::DrawStateDescriptor drawStateDesc =
            {
                renderObject->framebuffer,
                (shaderConstants.alphaMaskTexture != NULL) ? graphicObjects.psoAlphaMask : graphicObjects.pso,
                renderObject->viewport,
                gtf::scdr::DefaultScissor,
                0
            };
            // clang-format on

            executionContext_.setDrawState(drawStateDesc);

            executionContext_.setShaderConstant(*shaderConstants.object2Screen, renderObject->effectConstantBufferBlock.object2Screen, 16);
            executionContext_.setShaderConstant(*shaderConstants.color, &renderObject->effectConstantBufferBlock.color.r, 4);
            executionContext_.setShaderConstant(*shaderConstants.pos, renderObject->effectConstantBufferBlock.pos, 2);
            executionContext_.setShaderConstant(*shaderConstants.decay, &renderObject->effectConstantBufferBlock.decay, 1);
            executionContext_.setShaderConstant(*shaderConstants.density, &renderObject->effectConstantBufferBlock.density, 1);
            executionContext_.setShaderConstant(*shaderConstants.weight, &renderObject->effectConstantBufferBlock.weight, 1);
            executionContext_.setShaderConstant(*shaderConstants.samples, &renderObject->effectConstantBufferBlock.samples, 1);
            executionContext_.setTexture(*shaderConstants.colorTexture, 0, texture, graphicObjects.colorSampler);

            if (shaderConstants.alphaMaskTexture != NULL)
            {
                executionContext_.setTexture(*shaderConstants.alphaMaskTexture, 1,
                    renderObject->alphaMask->texture, renderObject->alphaMask->sampler);
                executionContext_.setShaderConstant(*shaderConstants.object2Alpha, renderObject->alphaMaskConstantBufferBlock.object2Alpha, 16);
            }

            const uint32_t offset = 0;
            const uint32_t stride = 4 * sizeof(float);
            executionContext_.setVertexBuffers(&graphicObjects.vb, &stride, &offset, 1);
            executionContext_.draw(4, 0);
        }

        return gtf::scdr::CommandErrorCode::Success;
    }

protected:
    CrepuscularRenderObject* renderObject;
};

struct CrepuscularVertex
{
    float pos[2];
    float tex[2];
};

// clang-format off
static const CrepuscularVertex CrepuscularQuad[] =
{
    { { 0.f, 0.f }, { 0.f, 1.f } },
    { { 0.f, 1.f }, { 0.f, 0.f } },
    { { 1.f, 0.f }, { 1.f, 1.f } },
    { { 1.f, 1.f }, { 1.f, 0.f } }
};
// clang-format on

static bool CreateCrepuscularQuad(gtf::scdr::GPUContext& context_, gtf::scdr::CommandBuffer& commandBuffer_, gtf::scdr::VertexBuffer*& vbOut_)
{
    vbOut_ = context_.createVertexBuffer(
        gtf::scdr::CBufferDescriptor(gtf::scdr::BufferUsage::Immutable, sizeof(CrepuscularQuad)),
        CrepuscularQuad,
        commandBuffer_);

    return vbOut_ != NULL;
}

static bool CreateCrepuscularShaderProgram(gtf::scdr::GraphicsApi driver_, gtf::scdr::GPUContext& context_, gtf::scdr::CommandBuffer& commandBuffer_, gtf::scdr::ShaderProgram*& progOut_)
{
    progOut_              = NULL;
    gtf::scdr::Shader* vs = NULL;
    gtf::scdr::Shader* fs = NULL;

    switch (driver_)
    {
    case gtf::scdr::GraphicsApi::OpenGLES3:
        vs = context_.createShader(gtf::scdr::Shader::Type::Vertex, VertexShaderGLES3, NULL, 0, commandBuffer_);
        fs = context_.createShader(gtf::scdr::Shader::Type::Fragment, FragmentShaderGLES3, NULL, 0, commandBuffer_);
        break;

    default:
        return false;
    }

    if (vs && fs)
    {
        const gtf::scdr::ShaderProgramSet progSet = { vs, fs };

        progOut_ = context_.createShaderProgram(progSet, commandBuffer_);

        context_.destroyShader(fs, commandBuffer_);
        context_.destroyShader(vs, commandBuffer_);
    }

    return progOut_ != NULL;
}

static gtf::scdr::ShaderProgram* CreateCrepuscularWithAlphaMaskShaderProgram(
    gtf::scdr::GraphicsApi    driver_,
    gtf::scdr::GPUContext&    context_,
    gtf::scdr::CommandBuffer& commandBuffer_)
{
    gtf::scdr::ShaderProgram* prog = NULL;
    gtf::scdr::Shader*        vs   = NULL;
    gtf::scdr::Shader*        fs   = NULL;

    switch (driver_)
    {
    case gtf::scdr::GraphicsApi::OpenGLES3:
        vs = context_.createShader(gtf::scdr::Shader::Type::Vertex, VertexShaderWithAlphaMaskGLES3, NULL, 0, commandBuffer_);
        fs = context_.createShader(gtf::scdr::Shader::Type::Fragment, FragmentShaderWithAlphaMaskGLES3, NULL, 0, commandBuffer_);
        break;

    default:
        return NULL;
    }

    if (vs && fs)
    {
        const gtf::scdr::ShaderProgramSet progSet = { vs, fs };

        prog = context_.createShaderProgram(progSet, commandBuffer_);

        context_.destroyShader(fs, commandBuffer_);
        context_.destroyShader(vs, commandBuffer_);

        return prog;
    }

    return NULL;
}

static bool CreateCrepuscularPso(gtf::scdr::ShaderProgram* prog_, gtf::scdr::GPUContext& context_, gtf::scdr::CommandBuffer& commandBuffer_, gtf::scdr::PipelineStateObject*& psoOut_)
{
    // clang-format off
    const gtf::scdr::PipelineInputDescriptor pid[] =
    {
        { "POSITION", gtf::scdr::VertexFormat::X32Y32_FLOAT, 0, 0, gtf::scdr::InputClassification::VertexData, 0 },
        { "TEXCOORD", gtf::scdr::VertexFormat::X32Y32_FLOAT, 0, 8, gtf::scdr::InputClassification::VertexData, 0 }
    };
    //clang-format on

    gtf::scdr::PipelineStateObjectDescriptor psod;
    psod.inputDescriptorsCount = ARRAY_SIZE(pid);
    psod.inputDescriptors      = pid;
    psod.program               = prog_;
    psod.primitiveTopology     = gtf::scdr::PrimitiveTopology::TriangleStrip;

    gtf::scdr::CreateAdditiveBlendStateDescriptor(psod.blendStateDescriptor);
    gtf::scdr::CreateDefaultRasterizerStateDescriptor(psod.rasterizerStateDescriptor);
    gtf::scdr::CreateDefaultDepthStencilStateDescriptor(psod.depthStencilStateDescriptor);

    psoOut_ = context_.createPipelineStateObject(psod, commandBuffer_);
    return (psoOut_ != NULL);
}

static bool CreateCrepuscularSampler(gtf::scdr::GPUContext& context_, gtf::scdr::CommandBuffer& commandBuffer_, gtf::scdr::SamplerState*& samplerOut_)
{
    const gtf::scdr::SamplerStateDescriptor desc = gtf::scdr::CSamplerStateDescriptor(gtf::scdr::TextureFilterMode::Point_MipNone);
    samplerOut_                                  = context_.createSamplerState(desc, commandBuffer_);

    return samplerOut_ != NULL;
}

static bool CreateCrepuscularShaderConstants(
    gtf::scdr::GraphicsApi driver_,
    gtf::scdr::ShaderProgram*     prog_,
    gtf::scdr::GPUContext&        gc_,
    gtf::scdr::CommandBuffer&     commandBuffer_,
    bool                          withAlphaMask,
    CrepuscularShaderConstants&   shaderConstants)
{
    if (driver_ != gtf::scdr::GraphicsApi::OpenGLES3)
    {
        return false;
    }

    shaderConstants.object2Screen = gc_.createShaderConstant("u_object2Screen", prog_, commandBuffer_);
    shaderConstants.color         = gc_.createShaderConstant("u_color", prog_, commandBuffer_);
    shaderConstants.pos           = gc_.createShaderConstant("u_pos", prog_, commandBuffer_);
    shaderConstants.decay         = gc_.createShaderConstant("decay", prog_, commandBuffer_);
    shaderConstants.density       = gc_.createShaderConstant("density", prog_, commandBuffer_);
    shaderConstants.weight        = gc_.createShaderConstant("weight", prog_, commandBuffer_);
    shaderConstants.samples       = gc_.createShaderConstant("samples", prog_, commandBuffer_);
    shaderConstants.colorTexture  = gc_.createShaderConstant("colorTexture", prog_, commandBuffer_);
    if (withAlphaMask)
    {
        shaderConstants.object2Alpha     = gc_.createShaderConstant("u_object2Alpha", prog_, commandBuffer_);
        shaderConstants.alphaMaskTexture = gc_.createShaderConstant("alphaMaskTexture", prog_, commandBuffer_);
    }
    return
        shaderConstants.object2Screen != NULL && shaderConstants.color != NULL &&
        shaderConstants.pos != NULL && shaderConstants.decay != NULL &&
        shaderConstants.density != NULL && shaderConstants.weight != NULL &&
        shaderConstants.samples != NULL && shaderConstants.colorTexture != NULL &&
        (!withAlphaMask || (shaderConstants.object2Alpha != NULL && shaderConstants.alphaMaskTexture != NULL));
}

static void DestroyShaderConstants(
    CrepuscularShaderConstants& shaderConstants,
    gtf::scdr::GPUContext&      gc_,
    gtf::scdr::CommandBuffer&   commandBuffer_)
{
    gc_.destroyShaderConstant(shaderConstants.object2Screen, commandBuffer_);
    gc_.destroyShaderConstant(shaderConstants.color, commandBuffer_);
    gc_.destroyShaderConstant(shaderConstants.pos, commandBuffer_);
    gc_.destroyShaderConstant(shaderConstants.decay, commandBuffer_);
    gc_.destroyShaderConstant(shaderConstants.density, commandBuffer_);
    gc_.destroyShaderConstant(shaderConstants.weight, commandBuffer_);
    gc_.destroyShaderConstant(shaderConstants.samples, commandBuffer_);
    gc_.destroyShaderConstant(shaderConstants.colorTexture, commandBuffer_);
    gc_.destroyShaderConstant(shaderConstants.object2Alpha, commandBuffer_);
    gc_.destroyShaderConstant(shaderConstants.alphaMaskTexture, commandBuffer_);
    memset(&shaderConstants, 0, sizeof(shaderConstants));
}

gtf::scdr::IWidgetRenderer* CrepuscularRenderer::createInstance(
    const gtf::scene::TypeResolver& resolver_,
    gtf::scdr::DataStore&           dataStore_,
    gtf::scdr::CommandBuffer&       commandBuffer_,
    gtf::scdr::Allocator&           frameAllocator_,
    const void*)
{
    BIND_ALLOCATOR(allocator, dataStore_);

    gtf::scdr::IWidgetRenderer* const renderer = ALLOC_OBJECT(allocator, CrepuscularRenderer, resolver_, dataStore_.getSystemConfig().getGraphicsApi());

    if (renderer)
    {
        if (renderer->init(resolver_, dataStore_, commandBuffer_, frameAllocator_))
        {
            return renderer;
        }

        renderer->deinit(dataStore_, commandBuffer_, frameAllocator_);
        FREE_OBJECT(allocator, renderer);
    }

    return NULL;
}

void CrepuscularRenderer::destroyInstance(
    gtf::scdr::IWidgetRenderer* renderer_,
    gtf::scdr::DataStore&       dataStore_,
    gtf::scdr::CommandBuffer&   commandBuffer_,
    gtf::scdr::Allocator&       frameAllocator_)
{
    if (renderer_)
    {
        renderer_->deinit(dataStore_, commandBuffer_, frameAllocator_);

        BIND_ALLOCATOR(allocator, dataStore_);
        FREE_OBJECT(allocator, static_cast<CrepuscularRenderer*>(renderer_));
    }
}

CrepuscularRenderer::CrepuscularRenderer(const gtf::scene::TypeResolver& resolver_, gtf::scdr::GraphicsApi driver_)
{
    gtf::scdr::ResolvePropertiesCommon2D(resolver_, CREPUSCULAR_PLUGIN_NAME, properties.common2d);
    gtf::scdr::ResolveFeaturesCommon2D(resolver_, features.common2d);

    memset(&graphicObjects, 0, sizeof(graphicObjects));
    graphicObjects.driver = driver_;

    // clang-format off
    gtf::scene::TypeResolver::ContainerPropertyReference wpr[] =
    {
        { { CREPUSCULAR_PLUGIN_NAME, gtf::properties::Container::Type() }, { "color"  , gtf::types::limits::uint16_max } },
        { { CREPUSCULAR_PLUGIN_NAME, gtf::properties::Container::Type() }, { "x-pos"  , gtf::types::limits::uint16_max } },
        { { CREPUSCULAR_PLUGIN_NAME, gtf::properties::Container::Type() }, { "y-pos"  , gtf::types::limits::uint16_max } },
        { { CREPUSCULAR_PLUGIN_NAME, gtf::properties::Container::Type() }, { "samples", gtf::types::limits::uint16_max } },
        { { CREPUSCULAR_PLUGIN_NAME, gtf::properties::Container::Type() }, { "decay"  , gtf::types::limits::uint16_max } },
        { { CREPUSCULAR_PLUGIN_NAME, gtf::properties::Container::Type() }, { "density", gtf::types::limits::uint16_max } },
        { { CREPUSCULAR_PLUGIN_NAME, gtf::properties::Container::Type() }, { "weight" , gtf::types::limits::uint16_max } }
    };
    // clang-format on

    resolver_.resolveContainerProperties(wpr, ARRAY_SIZE(wpr));

    properties.color   = wpr[0].property.identifier;
    properties.xpos    = wpr[1].property.identifier;
    properties.ypos    = wpr[2].property.identifier;
    properties.samples = wpr[3].property.identifier;
    properties.decay   = wpr[4].property.identifier;
    properties.density = wpr[5].property.identifier;
    properties.weight  = wpr[6].property.identifier;
}

CrepuscularRenderer::~CrepuscularRenderer()
{
    GTF_OSAL_ASSERT(graphicObjects.colorSampler == NULL);
    GTF_OSAL_ASSERT(graphicObjects.pso == NULL);
    GTF_OSAL_ASSERT(graphicObjects.vb == NULL);
}

bool CrepuscularRenderer::init(const gtf::scene::TypeResolver&, gtf::scdr::DataStore& dataStore_, gtf::scdr::CommandBuffer& commandBuffer_, gtf::scdr::Allocator&)
{
    BIND_GPU_CONTEXT(gc, dataStore_);

    bool                      success = true;
    gtf::scdr::ShaderProgram* program = NULL;

    success = CreateCrepuscularQuad(gc, commandBuffer_, this->graphicObjects.vb);
    if (success)
    {
        success = CreateCrepuscularShaderProgram(graphicObjects.driver, gc, commandBuffer_, program);
    }

    if (success)
    {
        success = CreateCrepuscularPso(program, gc, commandBuffer_, graphicObjects.pso);
    }

    if (success)
    {
        success = CreateCrepuscularShaderConstants(graphicObjects.driver, program, gc, commandBuffer_, false, graphicObjects.shaderConstants);
    }

    if (program)
    {
        gc.destroyShaderProgram(program, commandBuffer_);
        program = NULL;
    }

    program = CreateCrepuscularWithAlphaMaskShaderProgram(graphicObjects.driver, gc, commandBuffer_);

    if (program)
    {
        success = success ? CreateCrepuscularPso(program, gc, commandBuffer_, graphicObjects.psoAlphaMask) : false;
        success = success ? CreateCrepuscularShaderConstants(graphicObjects.driver, program, gc, commandBuffer_, true, graphicObjects.shaderConstantsAlphaMask) : false;

        gc.destroyShaderProgram(program, commandBuffer_);
    }
    else
    {
        success = false;
    }

    success = success ? CreateCrepuscularSampler(gc, commandBuffer_, graphicObjects.colorSampler) : false;
    success = success ? CreateCrepuscularSampler(gc, commandBuffer_, graphicObjects.alphaMaskSampler) : false;

    return success;
}

void CrepuscularRenderer::deinit(gtf::scdr::DataStore& dataStore_, gtf::scdr::CommandBuffer& commandBuffer_, gtf::scdr::Allocator&)
{
    BIND_GPU_CONTEXT(gc, dataStore_);

    gc.destroySamplerState(graphicObjects.alphaMaskSampler, commandBuffer_);
    gc.destroySamplerState(graphicObjects.colorSampler, commandBuffer_);
    gc.destroyPipelineStateObject(graphicObjects.psoAlphaMask, commandBuffer_);
    gc.destroyPipelineStateObject(graphicObjects.pso, commandBuffer_);
    gc.destroyVertexBuffer(graphicObjects.vb, commandBuffer_);
    DestroyShaderConstants(graphicObjects.shaderConstantsAlphaMask, gc, commandBuffer_);
    DestroyShaderConstants(graphicObjects.shaderConstants, gc, commandBuffer_);

    graphicObjects.colorSampler     = NULL;
    graphicObjects.alphaMaskSampler = NULL;
    graphicObjects.pso              = NULL;
    graphicObjects.psoAlphaMask     = NULL;
    graphicObjects.vb               = NULL;
}

bool CrepuscularRenderer::update(
    gtf::scdr::ResourceLoader&,
    gtf::scdr::DataStore&,
    gtf::scdr::CommandBuffer&,
    gtf::scdr::Allocator&)
{
    return true;
}

bool CrepuscularRenderer::render(
    gtf::scdr::DataStore&,
    gtf::scdr::CommandBuffer& commandBuffer_,
    gtf::scdr::Allocator&)
{
    for (CrepuscularRenderObject* object = objects; object; object = object->next)
    {
        if (gtf::scdr::Command* const clearCmd = ClearOffscreenFramebufferCommand::create(object->offscreenFramebuffer, commandBuffer_))
        {
            const gtf::scdr::ScheduleInfo clearInfo = gtf::scdr::CScheduleInfo(object->offscreenFbId, 0, gtf::scdr::ScheduleInfo::Clear);
            commandBuffer_.enqueueRender(clearInfo, clearCmd);

            gtf::scdr::Command*     drawCmd      = DrawEffectCommand::create(object, commandBuffer_);
            gtf::scdr::ScheduleInfo scheduleInfo = gtf::scdr::CScheduleInfo(object->alphaMask ? graphicObjects.psoAlphaMask : graphicObjects.pso,
                object->fbid, object->zid, gtf::scdr::ScheduleInfo::Transparent);

            if (drawCmd)
            {
                commandBuffer_.enqueueRender(scheduleInfo, drawCmd);
            }
        }
    }
    return true;
}

gtf::scdr::RenderObject* CrepuscularRenderer::createRenderObject(
    gtf::properties::Container& widget_,
    gtf::scdr::DataStore&       dataStore_,
    gtf::scdr::CommandBuffer&,
    gtf::scdr::Allocator&)
{
    BIND_ALLOCATOR(allocator, dataStore_);

    CrepuscularRenderObject* const object = ALLOC_OBJECT(allocator, CrepuscularRenderObject, graphicObjects, widget_, features.common2d, properties.common2d);
    return object;
}

void CrepuscularRenderer::destroyRenderObject(
    gtf::scdr::RenderObject*  object_,
    gtf::scdr::DataStore&     dataStore_,
    gtf::scdr::CommandBuffer& commandBuffer_,
    gtf::scdr::Allocator&)
{
    if (object_)
    {
        BIND_ALLOCATOR(allocator, dataStore_);
        BIND_GPU_CONTEXT(gc, dataStore_);

        CrepuscularRenderObject* const object = static_cast<CrepuscularRenderObject*>(object_);
        if (object->offscreenFramebuffer)
        {
            gc.destroyFramebuffer(object->offscreenFramebuffer, commandBuffer_);
        }

        FREE_OBJECT(allocator, object);
    }
}

static bool CreateOrUpdateFramebuffer(
    int32_t                       width_,
    int32_t                       height_,
    const gtf::scdr::Framebuffer* sysFb_,
    gtf::scdr::Framebuffer*&      offscreenFb_,
    gtf::scdr::DataStore&         dataStore_,
    gtf::scdr::CommandBuffer&     commandBuffer_)
{
    BIND_GPU_CONTEXT(gc, dataStore_);

    const uint32_t width  = static_cast<uint32_t>(width_);
    const uint32_t height = static_cast<uint32_t>(height_);

    // Avoid framebuffers with size 0/0
    if (0 == width_ || 0 == height_)
    {
        return false;
    }

    gtf::scdr::FramebufferDescriptor fbdesc;

    if (offscreenFb_)
    {
        fbdesc = offscreenFb_->getDescriptor();

        if (fbdesc.width == width && fbdesc.height == height)
        {
            return true;
        }

        gc.destroyFramebuffer(offscreenFb_, commandBuffer_);

        fbdesc.width  = width;
        fbdesc.height = height;
    }
    else
    {
        fbdesc         = sysFb_->getDescriptor();
        fbdesc.width   = width;
        fbdesc.height  = height;
        fbdesc.flags   = gtf::scdr::FramebufferFlags::ColorBufferAsTexture;
        fbdesc.samples = 1;
    }

    offscreenFb_ = gc.createFramebuffer(fbdesc, commandBuffer_);

    return offscreenFb_ != NULL;
}

bool CrepuscularRenderer::traverse(
    gtf::properties::Container&              widget_,
    const gtf::scdr::RenderObjectStackFrame& parentStackFrame_,
    gtf::scdr::RenderObjectStackFrame&       objectStackFrame_,
    gtf::scdr::RenderObject&                 object_,
    gtf::scdr::TraverserContext&             traverserContext_)
{
    gtf::scdr::DataStore&     dataStore     = traverserContext_.getDataStore();
    gtf::scdr::CommandBuffer& commandBuffer = traverserContext_.getCommandBuffer();

    CrepuscularStackFrame&          objectFrame          = static_cast<CrepuscularStackFrame&>(objectStackFrame_);
    CrepuscularRenderObject&        object               = static_cast<CrepuscularRenderObject&>(object_);
    CrepuscularConstantBufferBlock& effectParamsBlock    = object.effectConstantBufferBlock;
    AlphaMaskConstantBufferBlock&   alphaMaskParamsBlock = object.alphaMaskConstantBufferBlock;

    gtf::scdr::RenderObjectStackFrame2DValues stackValues;
    gtf::scdr::GetRenderObjectStackFrameValues2D(parentStackFrame_, stackValues);

    if (!object.resolveObjectParams(stackValues))
    {
        return false;
    }

    if (object.propertyValues.width <= 0 || object.propertyValues.height <= 0)
    {
        return false;
    }

    const uint32_t drawId = gtf::scdr::GetWidgetDepth(stackValues);

    const gtf::utils::math::float4x4 unit2Framebuffer = object.transforms.widget2Framebuffer * gtf::utils::math::float4x4::Translation(0.f, 0.f, static_cast<float>(drawId)) * gtf::utils::math::float4x4::Scaling2D(object.width, object.height);

    gtf::scdr::LoadValue(widget_, properties.color, effectParamsBlock.color);
    gtf::scdr::LoadValue(widget_, properties.decay, effectParamsBlock.decay);
    gtf::scdr::LoadValue(widget_, properties.density, effectParamsBlock.density);
    gtf::scdr::LoadValue(widget_, properties.samples, effectParamsBlock.samples);
    gtf::scdr::LoadValue(widget_, properties.weight, effectParamsBlock.weight);
    gtf::scdr::LoadValue(widget_, properties.xpos, effectParamsBlock.pos[0]);
    gtf::scdr::LoadValue(widget_, properties.ypos, effectParamsBlock.pos[1]);

    effectParamsBlock.color *= object.coloration;

    gtf::utils::math::float4x4::store(effectParamsBlock.object2Screen, (*stackValues.viewProjMat * unit2Framebuffer).transpose());
    gtf::utils::math::float4x4::store(alphaMaskParamsBlock.object2Alpha, (object.transforms.widget2AlphaMask * gtf::utils::math::float4x4::Scaling2D(object.width, object.height)).transpose());

    if (!CreateOrUpdateFramebuffer(object.propertyValues.width, object.propertyValues.height, stackValues.framebuffer, object.offscreenFramebuffer, dataStore, commandBuffer))
    {
        return false;
    }

    object.zid           = drawId;
    object.fbid          = stackValues.framebufferId;
    object.framebuffer   = stackValues.framebuffer;
    object.viewport      = *stackValues.viewport;
    object.offscreenFbId = traverserContext_.createFramebufferId();
    object.alphaMask     = stackValues.alphaMask;

    objectFrame.viewport           = gtf::scdr::CViewport(0, 0, static_cast<uint32_t>(object.propertyValues.width), static_cast<uint32_t>(object.propertyValues.height));
    objectFrame.scissor            = gtf::scdr::CScissor(0, stackValues.scissor->right - stackValues.scissor->left, 0, stackValues.scissor->bottom - stackValues.scissor->top);
    objectFrame.framebuffer2Screen = *stackValues.framebuffer2Screen * object.transforms.widget2Framebuffer;
    objectFrame.coloration         = object.coloration;
    objectFrame.framebuffer        = object.offscreenFramebuffer;
    objectFrame.framebufferId      = object.offscreenFbId;

    if (!gtf::scdr::InitializeDrawDepthForRenderObjectStackFrame(*object.offscreenFramebuffer, objectFrame.depth))
    {
        return false;
    }

    const uint32_t maxDepth = gtf::scdr::GetMaxFramebufferDepth(object.offscreenFramebuffer);
    if (maxDepth == 0)
    {
        return false;
    }

    gtf::scdr::CalculateViewProjectionMatrix2D(
        object.propertyValues.width,
        object.propertyValues.height,
        maxDepth,
        graphicObjects.driver,
        objectFrame.viewProjMat);

    if (object.featureValues.mask & gtf::scdr::WidgetFeaturesCommon2DValues::Viewport)
    {
        objectFrame.widget2Framebuffer = gtf::utils::math::float4x4::Translation2D(
            static_cast<float>(object.featureValues.viewportX),
            static_cast<float>(object.featureValues.viewportY));
    }
    else
    {
        objectFrame.widget2Framebuffer = gtf::utils::math::float4x4::Identity();
    }

    object.next = objects;
    objects     = &object;

    return true;
}

bool CrepuscularRenderer::begin(gtf::scdr::DataStore&, gtf::scdr::CommandBuffer&, gtf::scdr::Allocator&)
{
    this->objects = NULL;

    return true;
}

bool CrepuscularRenderer::end(gtf::scdr::DataStore&, gtf::scdr::CommandBuffer&, gtf::scdr::Allocator&)
{
    return true;
}
