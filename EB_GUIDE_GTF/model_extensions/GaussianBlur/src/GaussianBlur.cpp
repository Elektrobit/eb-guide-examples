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

#include "GaussianBlur.h"

#include "GLES20Shader.h"
#include "GLES3Shader.h"

#include <gtf/scdr/AlphaMask.h>
#include <gtf/scdr/RenderObjectBase.h>
#include <gtf/scdr/SDK.h>
#include <gtf/scdr/gpu/FramebufferClearFlags.h>
#include <gtf/scdr/gpu/GPUExecutionContext.h>
#include <gtf/tracing/Logging.h>
#include <gtf/types/Macros.h>
#include "GaussianBlurRenderObject.h"
#include "GaussianBlurCommand.h"

#define GTF_RENDERER_PLUGIN_GaussianBlur

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
        // clang-format off
        static const uint32_t flags = 
            gtf::scdr::FramebufferClearFlags::Color | 
            gtf::scdr::FramebufferClearFlags::Depth | 
            gtf::scdr::FramebufferClearFlags::Stencil;
        // clang-format on

        executionContext_.clearFramebuffer(fb, flags, gtf::scdr::SystemColors::Null, 1.0, 0);

        return gtf::scdr::CommandErrorCode::Success;
    }

private:
    gtf::scdr::Framebuffer* fb;
};

gtf::scdr::RenderObject* GaussianBlur::createRenderObject(gtf::properties::Container& widget_, gtf::scdr::DataStore& dataStore_, gtf::scdr::CommandBuffer&, gtf::scdr::Allocator&)
{
    BIND_ALLOCATOR(allocator, dataStore_);
    return ALLOC_OBJECT(allocator, GaussianBlurRenderObject, widget_, api, features.common2d, properties.common2d);
}

void GaussianBlur::destroyRenderObject(
    gtf::scdr::RenderObject*  object_,
    gtf::scdr::DataStore&     dataStore_,
    gtf::scdr::CommandBuffer& commandBuffer_,
    gtf::scdr::Allocator&)

{
    if (auto rob = static_cast<GaussianBlurRenderObject*>(object_))
    {
        BIND_ALLOCATOR(allocator, dataStore_);

        rob->releaseFramebuffers(dataStore_, commandBuffer_);

        FREE_OBJECT(allocator, rob);
    }
}

gtf::scdr::IWidgetRenderer* GaussianBlur::createInstance(const gtf::scene::TypeResolver& resolver_, gtf::scdr::DataStore& dataStore_, gtf::scdr::CommandBuffer& commandBuffer_, gtf::scdr::Allocator& frameAllocator_, const void*)
{
    BIND_ALLOCATOR(allocator, dataStore_);

    if (auto renderer = ALLOC_OBJECT(allocator, GaussianBlur, resolver_))
    {
        if (renderer->init(resolver_, dataStore_, commandBuffer_, frameAllocator_))
        {
            return renderer;
        }

        FREE_OBJECT(allocator, renderer);
    }
    return nullptr;
}

void GaussianBlur::destroyInstance(gtf::scdr::IWidgetRenderer* renderer_, gtf::scdr::DataStore& dataStore_, gtf::scdr::CommandBuffer& commandBuffer_, gtf::scdr::Allocator& frameAllocator_)
{
    if (renderer_)
    {
        BIND_ALLOCATOR(allocator, dataStore_);

        renderer_->deinit(dataStore_, commandBuffer_, frameAllocator_);

        return FREE_OBJECT(allocator, renderer_);
    }
}

GaussianBlur::GaussianBlur(const gtf::scene::TypeResolver& resolver_)
    : api(gtf::scdr::GraphicsApi::Unknown)
    , objects(nullptr)
{
    gtf::scdr::ResolvePropertiesCommon2D(resolver_, GAUSSIANBLUR, properties.common2d);
    gtf::scdr::ResolveFeaturesCommon2D(resolver_, features.common2d);

    // clang-format off
    gtf::scene::TypeResolver::ContainerPropertyReference wpr[] = 
    {
        { { GAUSSIANBLUR, gtf::properties::Container::Type() }, { "sigma"  , gtf::types::limits::uint16_max } },
        { { GAUSSIANBLUR, gtf::properties::Container::Type() }, { "enabled", gtf::types::limits::uint16_max } }
    };
    // clang-format on

    resolver_.resolveContainerProperties(wpr, ARRAY_SIZE(wpr));

    properties.sigma   = wpr[0].property.identifier;
    properties.enabled = wpr[1].property.identifier;

    memset(&graphicObjects, 0, sizeof(graphicObjects));
}

static gtf::scdr::PipelineStateObject* CreateBlitPipelineStateObject(gtf::scdr::ShaderProgram* program_, gtf::scdr::GPUContext& gc_, gtf::scdr::CommandBuffer& commandBuffer_)
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

static gtf::scdr::PipelineStateObject* CreateBlurPipelineStateObject(gtf::scdr::ShaderProgram* program_, gtf::scdr::GPUContext& gc_, gtf::scdr::CommandBuffer& commandBuffer_)
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
    gtf::scdr::CreateDefaultBlendStateDescriptor(desc.blendStateDescriptor);

    desc.rasterizerStateDescriptor.scissorEnable   = true;
    desc.depthStencilStateDescriptor.depthEnable   = false;
    desc.depthStencilStateDescriptor.stencilEnable = false;

    return gc_.createPipelineStateObject(desc, commandBuffer_);
}

bool GaussianBlur::isValid() const
{
    GTF_OSAL_ASSERT(api != gtf::scdr::GraphicsApi::Unknown);

    const auto& obj = graphicObjects;
    if (!obj.psoBlit)
        return false;
    if (!obj.psoAlphaBlit)
        return false;
    if (!obj.sampler0)
        return false;
    if (!obj.sampler1)
        return false;
    if (!obj.vertices)
        return false;

    if (api == gtf::scdr::GraphicsApi::OpenGLES3 && !obj.psoBlur)
    {
        return false;
    }
    else if (api == gtf::scdr::GraphicsApi::OpenGLES20 && obj.psoBlur)
    {
        return false;
    }

    return true;
}

static bool CreateShaderConstants(
    gtf::scdr::ShaderProgram* program_,
    gtf::scdr::GPUContext&    gc_,
    gtf::scdr::CommandBuffer& commandBuffer_,
    BlurPassShaderConstants&  shaderConstants)
{
    shaderConstants.texture0    = gc_.createShaderConstant("uTexture0", program_, commandBuffer_);
    shaderConstants.offsets     = gc_.createShaderConstant("uOffsets", program_, commandBuffer_);
    shaderConstants.weights     = gc_.createShaderConstant("uWeights", program_, commandBuffer_);
    shaderConstants.weightCount = gc_.createShaderConstant("uWeightCount", program_, commandBuffer_);
    shaderConstants.direction   = gc_.createShaderConstant("uDirection", program_, commandBuffer_);

    return shaderConstants.texture0 && shaderConstants.offsets && shaderConstants.weights && shaderConstants.weightCount && shaderConstants.direction;
}

static void DestroyShaderConstants(
    BlurPassShaderConstants&  shaderConstants,
    gtf::scdr::GPUContext&    gc_,
    gtf::scdr::CommandBuffer& commandBuffer_)
{
    gc_.destroyShaderConstant(shaderConstants.texture0, commandBuffer_);
    gc_.destroyShaderConstant(shaderConstants.offsets, commandBuffer_);
    gc_.destroyShaderConstant(shaderConstants.weights, commandBuffer_);
    gc_.destroyShaderConstant(shaderConstants.weightCount, commandBuffer_);
    gc_.destroyShaderConstant(shaderConstants.direction, commandBuffer_);

    memset(&shaderConstants, 0, sizeof(shaderConstants));
}

static bool CreateShaderConstants(
    gtf::scdr::ShaderProgram* program_,
    gtf::scdr::GPUContext&    gc_,
    gtf::scdr::CommandBuffer& commandBuffer_,
    bool                      withAlphaMask,
    BlitPassShaderConstants&  shaderConstants)
{
    shaderConstants.projMat = gc_.createShaderConstant("uProjMat", program_, commandBuffer_);
    if (!shaderConstants.projMat)
    {
        return false;
    }

    shaderConstants.modelMat = gc_.createShaderConstant("uModelMat", program_, commandBuffer_);
    if (!shaderConstants.modelMat)
    {
        return false;
    }

    shaderConstants.texture0 = gc_.createShaderConstant("uTexture0", program_, commandBuffer_);
    if (!shaderConstants.texture0)
    {
        return false;
    }

    if (withAlphaMask)
    {
        shaderConstants.object2Alpha = gc_.createShaderConstant("uObject2Alpha", program_, commandBuffer_);
        if (!shaderConstants.object2Alpha)
        {
            return false;
        }

        shaderConstants.texture1 = gc_.createShaderConstant("uTexture1", program_, commandBuffer_);
        if (!shaderConstants.texture1)
        {
            return false;
        }
    }

    return true;
}

static void DestroyShaderConstants(
    BlitPassShaderConstants&  shaderConstants,
    gtf::scdr::GPUContext&    gc_,
    gtf::scdr::CommandBuffer& commandBuffer_)
{
    gc_.destroyShaderConstant(shaderConstants.projMat, commandBuffer_);
    gc_.destroyShaderConstant(shaderConstants.modelMat, commandBuffer_);
    gc_.destroyShaderConstant(shaderConstants.object2Alpha, commandBuffer_);
    gc_.destroyShaderConstant(shaderConstants.texture0, commandBuffer_);
    gc_.destroyShaderConstant(shaderConstants.texture1, commandBuffer_);

    memset(&shaderConstants, 0, sizeof(shaderConstants));
}

bool GaussianBlur::init(const gtf::scene::TypeResolver&, gtf::scdr::DataStore& dataStore_, gtf::scdr::CommandBuffer& commandBuffer_, gtf::scdr::Allocator&)
{
    api = dataStore_.getSystemConfig().getGraphicsApi();

    BIND_GPU_CONTEXT(gc, dataStore_);

    gtf::scdr::Shader*        vsBlit   = nullptr;
    gtf::scdr::Shader*        fsBlit   = nullptr;
    gtf::scdr::ShaderProgram* progBlit = nullptr;

    gtf::scdr::Shader*        vsAlphaBlit   = nullptr;
    gtf::scdr::Shader*        fsAlphaBlit   = nullptr;
    gtf::scdr::ShaderProgram* progAlphaBlit = nullptr;

    gtf::scdr::Shader*        vsBlur   = nullptr;
    gtf::scdr::Shader*        fsBlur   = nullptr;
    gtf::scdr::ShaderProgram* progBlur = nullptr;

    switch (api)
    {
    case gtf::scdr::GraphicsApi::OpenGLES20:
        vsBlit      = gc.createShader(gtf::scdr::Shader::Type::Vertex, BlitVertexShaderGLES20, nullptr, 0, commandBuffer_);
        fsBlit      = gc.createShader(gtf::scdr::Shader::Type::Fragment, BlitFragmentShaderGLES20, nullptr, 0, commandBuffer_);
        vsAlphaBlit = gc.createShader(gtf::scdr::Shader::Type::Vertex, BlitVertexShaderWithAlphaMaskGLES20, nullptr, 0, commandBuffer_);
        fsAlphaBlit = gc.createShader(gtf::scdr::Shader::Type::Fragment, BlitFragmentShaderWithAlphaMaskGLES20, nullptr, 0, commandBuffer_);

        GTF_WARNING(GTF_RENDERER_PLUGIN_GaussianBlur, "Gaussian blur not supported under OpenGL ES 2.0. The effect will be permanently disabled!");
        break;

    case gtf::scdr::GraphicsApi::OpenGLES3:
        vsBlit      = gc.createShader(gtf::scdr::Shader::Type::Vertex, BlitVertexShaderGLES3, nullptr, 0, commandBuffer_);
        fsBlit      = gc.createShader(gtf::scdr::Shader::Type::Fragment, BlitFragmentShaderGLES3, nullptr, 0, commandBuffer_);
        vsAlphaBlit = gc.createShader(gtf::scdr::Shader::Type::Vertex, BlitVertexShaderWithAlphaMaskGLES3, nullptr, 0, commandBuffer_);
        fsAlphaBlit = gc.createShader(gtf::scdr::Shader::Type::Fragment, BlitFragmentShaderWithAlphaMaskGLES3, nullptr, 0, commandBuffer_);
        vsBlur      = gc.createShader(gtf::scdr::Shader::Type::Vertex, BlurVertexShaderGLES3, nullptr, 0, commandBuffer_);
        fsBlur      = gc.createShader(gtf::scdr::Shader::Type::Fragment, BlurFragmentShaderGLES3, nullptr, 0, commandBuffer_);
        break;

    default:
        return false;
    }

    if (vsBlit && fsBlit)
    {
        const gtf::scdr::ShaderProgramSet progSet = {
            vsBlit, fsBlit
        };

        progBlit = gc.createShaderProgram(progSet, commandBuffer_);
    }

    if (vsAlphaBlit && fsAlphaBlit)
    {
        const gtf::scdr::ShaderProgramSet progSet = {
            vsAlphaBlit, fsAlphaBlit
        };

        progAlphaBlit = gc.createShaderProgram(progSet, commandBuffer_);
    }

    if (vsBlur && fsBlur)
    {
        const gtf::scdr::ShaderProgramSet progSet = {
            vsBlur, fsBlur
        };

        progBlur = gc.createShaderProgram(progSet, commandBuffer_);
    }

    gc.destroyShader(vsBlit, commandBuffer_);
    gc.destroyShader(fsBlit, commandBuffer_);
    gc.destroyShader(vsAlphaBlit, commandBuffer_);
    gc.destroyShader(fsAlphaBlit, commandBuffer_);
    gc.destroyShader(vsBlur, commandBuffer_);
    gc.destroyShader(fsBlur, commandBuffer_);

    if (progBlit)
    {
        this->graphicObjects.psoBlit = CreateBlitPipelineStateObject(progBlit, gc, commandBuffer_);

        if (!CreateShaderConstants(progBlit, gc, commandBuffer_, false, graphicObjects.shaderConstants))
        {
            GTF_ERROR(GTF_RENDERER_PLUGIN_GaussianBlur, "Failed creating the blit pipeline state object.");
            return false;
        }
    }

    if (progAlphaBlit)
    {
        this->graphicObjects.psoAlphaBlit = CreateBlitPipelineStateObject(progAlphaBlit, gc, commandBuffer_);

        if (!CreateShaderConstants(progAlphaBlit, gc, commandBuffer_, true, graphicObjects.shaderConstantsWithAlphaMask))
        {
            GTF_ERROR(GTF_RENDERER_PLUGIN_GaussianBlur, "Failed creating the alpha mask blit pipeline state object.");
            return false;
        }
    }

    if (progBlur)
    {
        this->graphicObjects.psoBlur = CreateBlurPipelineStateObject(progBlur, gc, commandBuffer_);
        if (!CreateShaderConstants(progBlur, gc, commandBuffer_, graphicObjects.blurShaderConstants))
        {
            GTF_ERROR(GTF_RENDERER_PLUGIN_GaussianBlur, "Failed creating the blur pipeline state object.");
            return false;
        }
    }

    gc.destroyShaderProgram(progBlit, commandBuffer_);
    gc.destroyShaderProgram(progAlphaBlit, commandBuffer_);
    gc.destroyShaderProgram(progBlur, commandBuffer_);

    // Create a vertex buffer
    static GaussianBlurVertex vertices[] = {
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

    return isValid();
}

void GaussianBlur::deinit(gtf::scdr::DataStore& dataStore_, gtf::scdr::CommandBuffer& commandBuffer_, gtf::scdr::Allocator&)
{
    BIND_GPU_CONTEXT(gc, dataStore_);

    gc.destroySamplerState(graphicObjects.sampler1, commandBuffer_);
    gc.destroySamplerState(graphicObjects.sampler0, commandBuffer_);
    gc.destroyVertexBuffer(graphicObjects.vertices, commandBuffer_);
    gc.destroyPipelineStateObject(graphicObjects.psoAlphaBlit, commandBuffer_);
    gc.destroyPipelineStateObject(graphicObjects.psoBlit, commandBuffer_);
    gc.destroyPipelineStateObject(graphicObjects.psoBlur, commandBuffer_);
    DestroyShaderConstants(graphicObjects.shaderConstantsWithAlphaMask, gc, commandBuffer_);
    DestroyShaderConstants(graphicObjects.shaderConstants, gc, commandBuffer_);
    DestroyShaderConstants(graphicObjects.blurShaderConstants, gc, commandBuffer_);
}

bool GaussianBlur::begin(gtf::scdr::DataStore&, gtf::scdr::CommandBuffer&, gtf::scdr::Allocator&)
{
    // This method is called at the beginning of each frame rendered by the SCDR.
    // Reset the internal chain of custom widget data blocks. Since the memory blocks
    // behind these pointers are managed by the SCDR there is no need for cleanup here.
    objects = nullptr;
    return true;
}

bool GaussianBlur::traverse(
    gtf::properties::Container&              widget_,
    const gtf::scdr::RenderObjectStackFrame& parentStackFrame_,
    gtf::scdr::RenderObjectStackFrame&       objectStackFrame_,
    gtf::scdr::RenderObject&                 object_,
    gtf::scdr::TraverserContext&             traverserContext_)
{
    GaussianBlurRenderObject& object      = static_cast<GaussianBlurRenderObject&>(object_);
    GaussianBlurStackFrame&   objectFrame = static_cast<GaussianBlurStackFrame&>(objectStackFrame_);

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

    bool  enabled = false;
    float sigma   = 0.f;

    // Load effect specific parameters
    gtf::scdr::LoadValue(widget_, properties.enabled, enabled);
    gtf::scdr::LoadValue(widget_, properties.sigma, sigma);

    // Prepare the render object
    object.drawId                         = drawId;
    object.effectUniforms.projMat         = *stackValues.viewProjMat;
    object.effectUniforms.enabled         = enable(enabled);
    object.viewport                       = *stackValues.viewport;
    object.scissor                        = *stackValues.scissor;
    object.alphaMask                      = stackValues.alphaMask;
    object.alphaMaskUniforms.object2Alpha = object.transforms.widget2AlphaMask * gtf::utils::math::float4x4::Scaling2D(object.width, object.height);

    gtf::scdr::DataStore&     dataStore     = traverserContext_.getDataStore();
    gtf::scdr::CommandBuffer& commandBuffer = traverserContext_.getCommandBuffer();

    if (!object.updateFramebuffer(stackValues.framebuffer, object.propertyValues.width, object.propertyValues.height, dataStore, commandBuffer))
    {
        return false;
    }

    object.updateWeights(sigma);

    object.offscreenFramebufferId = traverserContext_.createFramebufferId();
    object.framebuffer            = stackValues.framebuffer;
    object.framebufferId          = stackValues.framebufferId;

    // Calculate the model matrix down here, in case the requested framebuffer size was not possible
    const auto fbDesc              = object.fbChildren->getDescriptor();
    object.effectUniforms.modelMat = object.transforms.widget2Framebuffer * gtf::utils::math::float4x4::Translation(0.f, 0.f, static_cast<float>(drawId)) * gtf::utils::math::float4x4::Scaling2D(static_cast<float>(fbDesc.width), static_cast<float>(fbDesc.height));

    // Prepare the stack frame for child widgets
    const uint32_t maxDepth = gtf::scdr::GetMaxFramebufferDepth(object.fbChildren);
    if (maxDepth == 0)
    {
        return false;
    }

    gtf::utils::math::float4x4 offscreenViewProjMat;
    if (!gtf::scdr::CalculateViewProjectionMatrix2D(object.propertyValues.width, object.propertyValues.height, maxDepth, api, offscreenViewProjMat))
    {
        return false;
    }

    uint32_t drawDepth;
    if (!gtf::scdr::InitializeDrawDepthForRenderObjectStackFrame(*object.fbChildren, drawDepth))
    {
        return false;
    }

    objectFrame.viewport           = gtf::scdr::CViewport(0, 0, object.propertyValues.width, object.propertyValues.height);
    objectFrame.scissor            = gtf::scdr::CScissor(0, gtf::types::limits::int16_max, 0, gtf::types::limits::int16_max);
    objectFrame.framebuffer        = object.fbChildren;
    objectFrame.framebufferId      = object.offscreenFramebufferId;
    objectFrame.viewProjMat        = offscreenViewProjMat;
    objectFrame.widget2Framebuffer = gtf::utils::math::float4x4::Identity();
    objectFrame.framebuffer2Screen = object.transforms.widget2Screen;
    objectFrame.coloration         = object.coloration;
    objectFrame.depth              = drawDepth;

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

bool GaussianBlur::update(gtf::scdr::ResourceLoader&, gtf::scdr::DataStore&, gtf::scdr::CommandBuffer&, gtf::scdr::Allocator&)
{
    return true;
}

bool GaussianBlur::render(gtf::scdr::DataStore&, gtf::scdr::CommandBuffer& commandBuffer_, gtf::scdr::Allocator&)
{
    gtf::scdr::Allocator& commandAllocator = commandBuffer_.getCommandAllocator();

    for (GaussianBlurRenderObject* object = objects; object != nullptr; object = object->next)
    {
        if (auto clearCmd = ALLOC_OBJECT(commandAllocator, ClearOffscreenFramebufferCommand, object->fbChildren))
        {
            const gtf::scdr::ScheduleInfo clearInfo = gtf::scdr::CScheduleInfo(object->offscreenFramebufferId, 0, gtf::scdr::ScheduleInfo::Clear);
            commandBuffer_.enqueueRender(clearInfo, clearCmd);

            if (auto drawCmd = ALLOC_OBJECT(commandAllocator, GaussianBlurCommand, &graphicObjects, object))
            {
                const gtf::scdr::ScheduleInfo scheduleInfo = gtf::scdr::CScheduleInfo(
                    object->alphaMask ? graphicObjects.psoBlit : graphicObjects.psoAlphaBlit,
                    object->framebufferId,
                    object->drawId,
                    gtf::scdr::CScheduleInfo::Transparent);

                if (drawCmd)
                {
                    commandBuffer_.enqueueRender(scheduleInfo, drawCmd);
                }
            }
        }
    }
    return true;
}

bool GaussianBlur::end(gtf::scdr::DataStore&, gtf::scdr::CommandBuffer&, gtf::scdr::Allocator&)
{
    // This method is called at the end of each frame rendered.
    // Reset the internal chain of custom widget data blocks. Since the memory blocks
    // behind these pointers are managed by the SCDR there is no need for cleanup here.
    return true;
}

bool GaussianBlur::enable(bool enabledPropety_) const
{
    return enabledPropety_ && (api == gtf::scdr::GraphicsApi::OpenGLES3);
}
