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

#include "RippleEffect.h"

#include "GLES20Shader.h"
#include "GLES3Shader.h"
#include "RippleEffectDesc.h"

#include <gtf/scdr/AlphaMask.h>
#include <gtf/scdr/RenderObjectBase.h>
#include <gtf/scdr/SDK.h>
#include <gtf/scdr/gpu/FramebufferClearFlags.h>
#include <gtf/scdr/gpu/GPUExecutionContext.h>
#include <gtf/types/Macros.h>

class RippleEffectStackFrame : public gtf::scdr::RenderObjectStackFrame2D
{
public:
    RippleEffectStackFrame()
        : framebuffer(NULL)
        , framebufferId(0)
        , depth(0)
        , widget2AlphaMask(gtf::utils::math::float4x4::Identity())
    {
        depthPtr = &depth;
    }

    virtual void load(gtf::scdr::RenderObjectStackFrame2DValues& valuesOut_) const
    {
        valuesOut_.coloration         = &coloration;
        valuesOut_.framebuffer        = framebuffer;
        valuesOut_.framebufferId      = framebufferId;
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
    gtf::scdr::Viewport        viewport;
    gtf::utils::math::float4x4 viewProjMat;
    gtf::utils::math::float4x4 framebuffer2Screen;
    gtf::utils::math::float4x4 widget2Framebuffer;
    gtf::scdr::Scissor         scissor;
    gtf::scdr::Color           coloration;
    gtf::scdr::Framebuffer*    framebuffer;
    uint32_t                   framebufferId;
    uint32_t                   depth;
    gtf::utils::math::float4x4 widget2AlphaMask;

private:
    uint32_t* depthPtr;
};

class RippleEffectRenderObject
    : public gtf::scdr::RenderObjectBase,
      public gtf::scdr::TouchObject
{
public:
    RippleEffectRenderObject(gtf::properties::Container& model_, const gtf::scdr::WidgetFeaturesCommon2D& features_, const gtf::scdr::WidgetPropertiesCommon2D& properties_)
        : RenderObjectBase(model_, features_, properties_)
        , next(NULL)
        , framebuffer(NULL)
        , offscreenFramebuffer(NULL)
        , framebufferId(0)
        , offscreenFramebufferId(0)
        , drawId(0)
        , alphaMask(NULL)
    {
    }

    gtf::scdr::RenderObjectStackFrame* createStackFrame(gtf::scdr::RenderObjectStackFrame& parent_, gtf::scdr::Allocator& allocator_)
    {
        GTF_UNUSED_PARAM(parent_);

        return ALLOC_OBJECT(allocator_, RippleEffectStackFrame);
    }

    void destroyStackFrame(gtf::scdr::RenderObjectStackFrame* frame_, gtf::scdr::Allocator& allocator_)
    {
        FREE_OBJECT(allocator_, static_cast<RippleEffectStackFrame*>(frame_));
    }

    const gtf::scdr::TouchObject* getTouchObject()
    {
        if (featureValues.mask & gtf::scdr::WidgetFeaturesCommon2DValues::Touch)
        {
            return this;
        }
        return NULL;
    }

    virtual void getBoundingRectangle(gtf::scdr::Rectangle<int16_t>& rectOut_) const
    {
        gtf::scdr::CalculateBoundingRectangle(*this, rectOut_);
    }

    virtual bool intersect(const gtf::scdr::ScreenPosition& pos_) const
    {
        return gtf::scdr::IntersectsBoundingRectangle(*this, pos_);
    }

public:
    RippleEffectRenderObject*   next;
    gtf::scdr::Viewport         viewport;
    gtf::scdr::Framebuffer*     framebuffer;
    gtf::scdr::Framebuffer*     offscreenFramebuffer;
    uint32_t                    framebufferId;
    uint32_t                    offscreenFramebufferId;
    uint32_t                    drawId;
    RippleEffectUniforms        effectUniforms;
    AlphaMaskUniforms           alphaMaskUniforms;
    const gtf::scdr::AlphaMask* alphaMask;
};

struct RippleVertex
{
    float posX;
    float posY;
    float texU;
    float texV;
};

gtf::scdr::RenderObject* RippleEffect::createRenderObject(gtf::properties::Container& widget_, gtf::scdr::DataStore& dataStore_, gtf::scdr::CommandBuffer&, gtf::scdr::Allocator&)
{
    BIND_ALLOCATOR(allocator, dataStore_);
    return ALLOC_OBJECT(allocator, RippleEffectRenderObject, widget_, features.common2d, properties.common2d);
}

void RippleEffect::destroyRenderObject(
    gtf::scdr::RenderObject*  object_,
    gtf::scdr::DataStore&     dataStore_,
    gtf::scdr::CommandBuffer& commandBuffer_,
    gtf::scdr::Allocator&)

{
    BIND_GPU_CONTEXT(gc, dataStore_);
    BIND_ALLOCATOR(allocator, dataStore_);

    gtf::scdr::Framebuffer* fb = static_cast<RippleEffectRenderObject*>(object_)->offscreenFramebuffer;
    if (fb)
    {
        gc.destroyFramebuffer(fb, commandBuffer_);
    }

    FREE_OBJECT(allocator, static_cast<RippleEffectRenderObject*>(object_));
}

gtf::scdr::IWidgetRenderer* RippleEffect::createInstance(const gtf::scene::TypeResolver& resolver_, gtf::scdr::DataStore& dataStore_, gtf::scdr::CommandBuffer& commandBuffer_, gtf::scdr::Allocator& frameAllocator_, const void*)
{
    BIND_ALLOCATOR(allocator, dataStore_);

    gtf::scdr::IWidgetRenderer* const renderer = ALLOC_OBJECT(allocator, RippleEffect, resolver_);
    if (renderer)
    {
        if (renderer->init(resolver_, dataStore_, commandBuffer_, frameAllocator_))
        {
            return renderer;
        }

        FREE_OBJECT(allocator, renderer);
    }

    return NULL;
}

void RippleEffect::destroyInstance(gtf::scdr::IWidgetRenderer* renderer_, gtf::scdr::DataStore& dataStore_, gtf::scdr::CommandBuffer& commandBuffer_, gtf::scdr::Allocator& frameAllocator_)
{
    BIND_ALLOCATOR(allocator, dataStore_);
    if (renderer_)
    {
        renderer_->deinit(dataStore_, commandBuffer_, frameAllocator_);
    }

    return FREE_OBJECT(allocator, static_cast<RippleEffect*>(renderer_));
}

RippleEffect::RippleEffect(const gtf::scene::TypeResolver& resolver_)
    : objects(NULL)
{
    gtf::scdr::ResolvePropertiesCommon2D(resolver_, RippleEffectDesc::WidgetName, properties.common2d);
    gtf::scdr::ResolveFeaturesCommon2D(resolver_, features.common2d);

    // clang-format off
    gtf::scene::TypeResolver::ContainerPropertyReference wpr[] = 
    {
        { { RippleEffectDesc::WidgetName, gtf::properties::Container::Type() }, { "animationTime", gtf::types::limits::uint16_max } },
        { { RippleEffectDesc::WidgetName, gtf::properties::Container::Type() }, { "enabled", gtf::types::limits::uint16_max       } }
    };
    // clang-format on

    resolver_.resolveContainerProperties(wpr, ARRAY_SIZE(wpr));

    properties.animationTime = wpr[0].property.identifier;
    properties.enabled       = wpr[1].property.identifier;

    memset(&graphicObjects, 0, sizeof(graphicObjects));
}

RippleEffect::~RippleEffect()
{
}

static gtf::scdr::PipelineStateObject* CreatePipelineStateObject(gtf::scdr::ShaderProgram* program_, gtf::scdr::GPUContext& gc_, gtf::scdr::CommandBuffer& commandBuffer_)
{
    // clang-format off
    static const gtf::scdr::PipelineInputDescriptor inputDesc[] =
    {
        { "aPosition", gtf::scdr::VertexFormat::X32Y32_FLOAT, 0, 0                , gtf::scdr::InputClassification::VertexData, 0 },
        { "aTexCoord", gtf::scdr::VertexFormat::X32Y32_FLOAT, 0, 2 * sizeof(float), gtf::scdr::InputClassification::VertexData, 0 }
    };
    // clang-format on

    gtf::scdr::PipelineStateObjectDescriptor desc;
    desc.inputDescriptors      = inputDesc;
    desc.inputDescriptorsCount = ARRAY_SIZE(inputDesc);
    desc.program               = program_;
    desc.primitiveTopology     = gtf::scdr::PrimitiveTopology::TriangleStrip;

    gtf::scdr::CreateDefaultDepthStencilStateDescriptor(desc.depthStencilStateDescriptor);
    gtf::scdr::CreateDefaultRasterizerStateDescriptor(desc.rasterizerStateDescriptor);
    gtf::scdr::CreateAdditiveBlendStateDescriptor(desc.blendStateDescriptor);

    desc.rasterizerStateDescriptor.scissorEnable = true;

    return gc_.createPipelineStateObject(desc, commandBuffer_);
}

static bool IsValid(const RippleEffectGraphicObjects& objects_)
{
    if (!objects_.psoEffect)
        return false;
    if (!objects_.psoWithAlphaMask)
        return false;
    if (!objects_.sampler0)
        return false;
    if (!objects_.sampler1)
        return false;
    if (!objects_.vertices)
        return false;

    return true;
}

static bool CreateShaderConstants(
    gtf::scdr::ShaderProgram*    program_,
    gtf::scdr::GPUContext&       gc_,
    gtf::scdr::CommandBuffer&    commandBuffer_,
    bool                         withAlphaMask,
    RippleEffectShaderConstants& shaderConstants)
{
    shaderConstants.projMat      = gc_.createShaderConstant("uProjMat", program_, commandBuffer_);
    shaderConstants.modelMat     = gc_.createShaderConstant("uModelMat", program_, commandBuffer_);
    shaderConstants.rippleTime   = gc_.createShaderConstant("uRippleTime", program_, commandBuffer_);
    shaderConstants.rippleEnable = gc_.createShaderConstant("uRippleEnable", program_, commandBuffer_);
    shaderConstants.texture0     = gc_.createShaderConstant("u_texture0", program_, commandBuffer_);
    if (withAlphaMask)
    {
        shaderConstants.object2Alpha = gc_.createShaderConstant("uObject2Alpha", program_, commandBuffer_);
        shaderConstants.texture1     = gc_.createShaderConstant("u_texture1", program_, commandBuffer_);
    }
    return shaderConstants.projMat != NULL && shaderConstants.modelMat != NULL && shaderConstants.rippleTime != NULL && shaderConstants.texture0 != NULL && (!withAlphaMask || (shaderConstants.object2Alpha != NULL && shaderConstants.texture1 != NULL));
}

static void DestroyShaderConstants(
    RippleEffectShaderConstants& shaderConstants,
    gtf::scdr::GPUContext&       gc_,
    gtf::scdr::CommandBuffer&    commandBuffer_)
{
    gc_.destroyShaderConstant(shaderConstants.projMat, commandBuffer_);
    gc_.destroyShaderConstant(shaderConstants.modelMat, commandBuffer_);
    gc_.destroyShaderConstant(shaderConstants.rippleTime, commandBuffer_);
    gc_.destroyShaderConstant(shaderConstants.rippleEnable, commandBuffer_);
    gc_.destroyShaderConstant(shaderConstants.object2Alpha, commandBuffer_);
    gc_.destroyShaderConstant(shaderConstants.texture0, commandBuffer_);
    gc_.destroyShaderConstant(shaderConstants.texture1, commandBuffer_);
    memset(&shaderConstants, 0, sizeof(shaderConstants));
}

bool RippleEffect::init(const gtf::scene::TypeResolver&, gtf::scdr::DataStore& dataStore_, gtf::scdr::CommandBuffer& commandBuffer_, gtf::scdr::Allocator&)
{
    const gtf::scdr::GraphicsApi api = dataStore_.getSystemConfig().getGraphicsApi();

    BIND_GPU_CONTEXT(gc, dataStore_);

    gtf::scdr::Shader*        vs   = NULL;
    gtf::scdr::Shader*        fs   = NULL;
    gtf::scdr::ShaderProgram* prog = NULL;

    gtf::scdr::Shader*        vsAlphaMask   = NULL;
    gtf::scdr::Shader*        fsAlphaMask   = NULL;
    gtf::scdr::ShaderProgram* progAlphaMask = NULL;

    switch (api)
    {
    case gtf::scdr::GraphicsApi::OpenGLES20:
        vs          = gc.createShader(gtf::scdr::Shader::Type::Vertex, VertexShaderGLES20, NULL, 0, commandBuffer_);
        fs          = gc.createShader(gtf::scdr::Shader::Type::Fragment, FragmentShaderGLES20, NULL, 0, commandBuffer_);
        vsAlphaMask = gc.createShader(gtf::scdr::Shader::Type::Vertex, VertexShaderWithAlphaMaskGLES20, NULL, 0, commandBuffer_);
        fsAlphaMask = gc.createShader(gtf::scdr::Shader::Type::Fragment, FragmentShaderWithAlphaMaskGLES20, NULL, 0, commandBuffer_);
        break;

    case gtf::scdr::GraphicsApi::OpenGLES3:
        vs          = gc.createShader(gtf::scdr::Shader::Type::Vertex, VertexShaderGLES3, NULL, 0, commandBuffer_);
        fs          = gc.createShader(gtf::scdr::Shader::Type::Fragment, FragmentShaderGLES3, NULL, 0, commandBuffer_);
        vsAlphaMask = gc.createShader(gtf::scdr::Shader::Type::Vertex, VertexShaderWithAlphaMaskGLES3, NULL, 0, commandBuffer_);
        fsAlphaMask = gc.createShader(gtf::scdr::Shader::Type::Fragment, FragmentShaderWithAlphaMaskGLES3, NULL, 0, commandBuffer_);
        break;

    default:
        return false;
    }

    if (vs && fs)
    {
        const gtf::scdr::ShaderProgramSet progSet = {
            vs, fs
        };

        prog = gc.createShaderProgram(progSet, commandBuffer_);
    }

    if (vsAlphaMask && fsAlphaMask)
    {
        const gtf::scdr::ShaderProgramSet progSet = {
            vsAlphaMask, fsAlphaMask
        };

        progAlphaMask = gc.createShaderProgram(progSet, commandBuffer_);
    }

    gc.destroyShader(vs, commandBuffer_);
    gc.destroyShader(fs, commandBuffer_);
    gc.destroyShader(vsAlphaMask, commandBuffer_);
    gc.destroyShader(fsAlphaMask, commandBuffer_);

    if (prog)
    {
        this->graphicObjects.psoEffect = CreatePipelineStateObject(prog, gc, commandBuffer_);

        if (!CreateShaderConstants(prog, gc, commandBuffer_, false, graphicObjects.shaderConstants))
        {
            return false;
        }
    }

    if (progAlphaMask)
    {
        this->graphicObjects.psoWithAlphaMask = CreatePipelineStateObject(progAlphaMask, gc, commandBuffer_);

        if (!CreateShaderConstants(progAlphaMask, gc, commandBuffer_, true, graphicObjects.shaderConstantsWithAlphaMask))
        {
            return false;
        }
    }

    gc.destroyShaderProgram(prog, commandBuffer_);
    gc.destroyShaderProgram(progAlphaMask, commandBuffer_);

    // Create a vertex buffer
    static RippleVertex vertices[] = {
        { 0.f, 0.f, 0.f, 0.f },
        { 0.f, 1.f, 0.f, 1.f },
        { 1.f, 0.f, 1.f, 0.f },
        { 1.f, 1.f, 1.f, 1.f }
    };

    this->graphicObjects.vertices = gc.createVertexBuffer(
        gtf::scdr::CBufferDescriptor(gtf::scdr::BufferUsage::Immutable, sizeof(vertices)),
        vertices,
        commandBuffer_);

    this->graphicObjects.sampler0 = gc.createSamplerState(gtf::scdr::CSamplerStateDescriptor(gtf::scdr::TextureFilterMode::Linear_MipNone), commandBuffer_);
    this->graphicObjects.sampler1 = gc.createSamplerState(gtf::scdr::CSamplerStateDescriptor(gtf::scdr::TextureFilterMode::Linear_MipNone), commandBuffer_);

    return IsValid(this->graphicObjects);
}

void RippleEffect::deinit(gtf::scdr::DataStore& dataStore_, gtf::scdr::CommandBuffer& commandBuffer_, gtf::scdr::Allocator&)
{
    BIND_GPU_CONTEXT(gc, dataStore_);

    gc.destroySamplerState(graphicObjects.sampler1, commandBuffer_);
    gc.destroySamplerState(graphicObjects.sampler0, commandBuffer_);
    gc.destroyVertexBuffer(graphicObjects.vertices, commandBuffer_);
    gc.destroyPipelineStateObject(graphicObjects.psoWithAlphaMask, commandBuffer_);
    gc.destroyPipelineStateObject(graphicObjects.psoEffect, commandBuffer_);
    DestroyShaderConstants(graphicObjects.shaderConstantsWithAlphaMask, gc, commandBuffer_);
    DestroyShaderConstants(graphicObjects.shaderConstants, gc, commandBuffer_);
}

bool RippleEffect::begin(gtf::scdr::DataStore&, gtf::scdr::CommandBuffer&, gtf::scdr::Allocator&)
{
    // This method is called at the beginning of each frame rendered by the SCDR.
    // Reset the internal chain of custom widget data blocks. Since the memory blocks
    // behind these pointers are managed by the SCDR there is no need for cleanup here.
    objects = NULL;
    return true;
}

bool RippleEffect::traverse(
    gtf::properties::Container&              widget_,
    const gtf::scdr::RenderObjectStackFrame& parentStackFrame_,
    gtf::scdr::RenderObjectStackFrame&       objectStackFrame_,
    gtf::scdr::RenderObject&                 object_,
    gtf::scdr::TraverserContext&             traverserContext_)
{
    RippleEffectRenderObject& object      = static_cast<RippleEffectRenderObject&>(object_);
    RippleEffectStackFrame&   objectFrame = static_cast<RippleEffectStackFrame&>(objectStackFrame_);

    // Load common common parameters and values
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

    bool    enabled = false;
    int32_t time    = 0;

    // Load effect specific parameters
    gtf::scdr::LoadValue(widget_, properties.enabled, enabled);
    gtf::scdr::LoadValue(widget_, properties.animationTime, time);

    // Prepare the render object
    object.drawId                         = drawId;
    object.effectUniforms.rippleEnable    = enabled ? 1.f : 0.f;
    object.effectUniforms.rippleTime      = static_cast<float>(time);
    object.effectUniforms.modelMat        = object.transforms.widget2Framebuffer * gtf::utils::math::float4x4::Translation(0.f, 0.f, static_cast<float>(drawId)) * gtf::utils::math::float4x4::Scaling2D(object.width, object.height);
    object.effectUniforms.projMat         = *stackValues.viewProjMat;
    object.viewport                       = *stackValues.viewport;
    object.alphaMask                      = stackValues.alphaMask;
    object.alphaMaskUniforms.object2Alpha = object.transforms.widget2AlphaMask * gtf::utils::math::float4x4::Scaling2D(object.width, object.height);

    gtf::scdr::DataStore&     dataStore     = traverserContext_.getDataStore();
    gtf::scdr::CommandBuffer& commandBuffer = traverserContext_.getCommandBuffer();

    if (!createOrUpdateFramebuffer(object.propertyValues.width, object.propertyValues.height, stackValues.framebuffer, object.offscreenFramebuffer, dataStore, commandBuffer))
    {
        return false;
    }

    object.offscreenFramebufferId = traverserContext_.createFramebufferId();
    object.framebuffer            = stackValues.framebuffer;
    object.framebufferId          = stackValues.framebufferId;

    // Prepare the stack frame for child widgets
    const uint32_t maxDepth = gtf::scdr::GetMaxFramebufferDepth(object.offscreenFramebuffer);
    if (maxDepth == 0)
    {
        return false;
    }

    gtf::utils::math::float4x4          offscreenViewProjMat;
    const gtf::scdr::GraphicsApi api = dataStore.getSystemConfig().getGraphicsApi();
    if (!gtf::scdr::CalculateViewProjectionMatrix2D(object.propertyValues.width, object.propertyValues.height, maxDepth, api, offscreenViewProjMat))
    {
        return false;
    }

    objectFrame.viewport           = gtf::scdr::CViewport(0, 0, object.propertyValues.width, object.propertyValues.height);
    objectFrame.scissor            = gtf::scdr::CScissor(0, gtf::types::limits::int16_max, 0, gtf::types::limits::int16_max);
    objectFrame.framebuffer        = object.offscreenFramebuffer;
    objectFrame.framebufferId      = object.offscreenFramebufferId;
    objectFrame.viewProjMat        = offscreenViewProjMat;
    objectFrame.widget2Framebuffer = gtf::utils::math::float4x4::Identity();
    objectFrame.framebuffer2Screen = object.transforms.widget2Screen;
    objectFrame.coloration         = object.coloration;
    objectFrame.depth              = maxDepth - 1;

    if (object.featureValues.mask & gtf::scdr::WidgetFeaturesCommon2DValues::Viewport)
    {
        objectFrame.widget2Framebuffer = gtf::utils::math::float4x4::Translation2D(
            static_cast<float>(object.featureValues.viewportX),
            static_cast<float>(object.featureValues.viewportY));
    }

    // queue the render object for drawing
    object.next = objects;
    objects     = &object;

    return true;
}

bool RippleEffect::update(gtf::scdr::ResourceLoader&, gtf::scdr::DataStore&, gtf::scdr::CommandBuffer&, gtf::scdr::Allocator&)
{
    return true;
}

class ClearOffscreenFramebufferCommand : public gtf::scdr::Command
{
public:
    ClearOffscreenFramebufferCommand(gtf::scdr::Framebuffer* fb_)
        : fb(fb_)
    {
    }

public:
    uint32_t execute(gtf::scdr::GPUExecutionContext& executionContext_, gtf::scdr::Allocator&)
    {
        static const uint32_t flags = gtf::scdr::FramebufferClearFlags::Color | gtf::scdr::FramebufferClearFlags::Depth | gtf::scdr::FramebufferClearFlags::Stencil;

        executionContext_.clearFramebuffer(fb, flags, gtf::scdr::SystemColors::Null, 1.0, 0);

        return gtf::scdr::CommandErrorCode::Success;
    }

private:
    gtf::scdr::Framebuffer* fb;
};

class DrawEffectCommand : public gtf::scdr::Command
{
public:
    DrawEffectCommand(RippleEffectGraphicObjects* graphicObjects_, RippleEffectRenderObject* renderObject_)
        : graphicObjects(graphicObjects_)
        , renderObject(renderObject_)
    {
    }

    uint32_t execute(gtf::scdr::GPUExecutionContext& executionContext_, gtf::scdr::Allocator&)
    {
        uint32_t stride = sizeof(RippleVertex);
        uint32_t offset = 0;

        gtf::scdr::Texture2D* const colorTexture = renderObject->offscreenFramebuffer->getColorAttachment();

        const RippleEffectShaderConstants& shaderConstants = (renderObject->alphaMask != NULL)
                                                                 ? graphicObjects->shaderConstantsWithAlphaMask
                                                                 : graphicObjects->shaderConstants;

        // clang-format off
        const gtf::scdr::DrawStateDescriptor drawStateDesc =
        {
            renderObject->framebuffer,
            (renderObject->alphaMask != NULL) ? graphicObjects->psoWithAlphaMask : graphicObjects->psoEffect,
            renderObject->viewport,
            renderObject->scissor,
            0
        };
        // clang-format on

        executionContext_.setDrawState(drawStateDesc);

        executionContext_.setShaderConstant(*shaderConstants.projMat, renderObject->effectUniforms.projMat.values, 16);
        executionContext_.setShaderConstant(*shaderConstants.modelMat, renderObject->effectUniforms.modelMat.values, 16);
        executionContext_.setShaderConstant(*shaderConstants.rippleTime, &renderObject->effectUniforms.rippleTime, 1);
        executionContext_.setShaderConstant(*shaderConstants.rippleEnable, &renderObject->effectUniforms.rippleEnable, 1);
        executionContext_.setTexture(*shaderConstants.texture0, 0, colorTexture, graphicObjects->sampler0);

        if (shaderConstants.texture1 != NULL)
        {
            executionContext_.setTexture(*shaderConstants.texture1, 1, renderObject->alphaMask->texture, renderObject->alphaMask->sampler);
            executionContext_.setShaderConstant(*shaderConstants.object2Alpha, renderObject->alphaMaskUniforms.object2Alpha.values, 16);
        }

        executionContext_.setVertexBuffers(&(graphicObjects->vertices), &stride, &offset, 1);
        executionContext_.draw(4, 0);

        return gtf::scdr::CommandErrorCode::Success;
    }

protected:
    RippleEffectGraphicObjects* graphicObjects;
    RippleEffectRenderObject*   renderObject;
};

bool RippleEffect::render(gtf::scdr::DataStore&, gtf::scdr::CommandBuffer& commandBuffer_, gtf::scdr::Allocator&)
{
    gtf::scdr::Allocator& commandAllocator = commandBuffer_.getCommandAllocator();

    for (RippleEffectRenderObject* object = objects; object != NULL; object = object->next)
    {
        if (gtf::scdr::Command* const clearCmd = ALLOC_OBJECT(commandAllocator, ClearOffscreenFramebufferCommand, object->offscreenFramebuffer))
        {
            const gtf::scdr::ScheduleInfo clearInfo = gtf::scdr::CScheduleInfo(object->offscreenFramebufferId, 0, gtf::scdr::ScheduleInfo::Clear);
            commandBuffer_.enqueueRender(clearInfo, clearCmd);

            gtf::scdr::Command* const drawCmd = ALLOC_OBJECT(commandAllocator, DrawEffectCommand, &graphicObjects, object);

            const gtf::scdr::ScheduleInfo scheduleInfo = gtf::scdr::CScheduleInfo(
                object->alphaMask ? graphicObjects.psoWithAlphaMask : graphicObjects.psoEffect,
                object->framebufferId,
                object->drawId,
                gtf::scdr::CScheduleInfo::Transparent);

            if (drawCmd)
            {
                commandBuffer_.enqueueRender(scheduleInfo, drawCmd);
            }
        }
    }
    return true;
}

bool RippleEffect::end(gtf::scdr::DataStore&, gtf::scdr::CommandBuffer&, gtf::scdr::Allocator&)
{
    // This method is called at the end of each frame rendered.
    // Reset the internal chain of custom widget data blocks. Since the memory blocks
    // behind these pointers are managed by the SCDR there is no need for cleanup here.
    return true;
}

bool RippleEffect::createOrUpdateFramebuffer(
    int32_t                       width_,
    int32_t                       height_,
    const gtf::scdr::Framebuffer* sysFb_,
    gtf::scdr::Framebuffer*&      fbInOut_,
    gtf::scdr::DataStore&         dataStore_,
    gtf::scdr::CommandBuffer&     commandBuffer_)
{
    BIND_GPU_CONTEXT(gc, dataStore_);

    uint32_t width  = static_cast<uint32_t>(width_);
    uint32_t height = static_cast<uint32_t>(height_);

    // Avoid framebuffers with size 0/0
    if (0 == width_ || 0 == height_)
    {
        return false;
    }

    gtf::scdr::FramebufferDescriptor fbdesc;

    if (fbInOut_)
    {
        fbdesc = fbInOut_->getDescriptor();

        if (fbdesc.width == width && fbdesc.height == height)
        {
            return true;
        }

        gc.destroyFramebuffer(fbInOut_, commandBuffer_);

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

    fbInOut_ = gc.createFramebuffer(fbdesc, commandBuffer_);

    return fbInOut_ != NULL;
}
