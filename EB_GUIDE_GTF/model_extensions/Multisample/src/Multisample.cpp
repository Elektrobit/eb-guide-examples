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

#include <gtf/scdr/AlphaMask.h>
#include <gtf/scdr/RenderObject.h>
#include <gtf/scdr/RenderObjectBase.h>
#include <gtf/scdr/SDK.h>
#include <gtf/scdr/gpu/Scissor.h>
#include <gtf/scdr/gpu/BlendStateDescriptor.h>
#include <gtf/scdr/gpu/BufferDescriptor.h>
#include <gtf/scdr/gpu/DepthStencilStateDescriptor.h>
#include <gtf/scdr/gpu/RasterizerStateDescriptor.h>

#include <gtf/tracing/Logging.h>
#include <gtf/types/Macros.h>

#include "GLES20Shader.h"
#include "GLES3Shader.h"
#include "Multisample.h"
#include "MultisampleCommand.h"

#define GTF_RENDERER_PLUGIN_Multisample

typedef MultisampleVertex       Vertex;
typedef MultisampleVertexWithUV VertexWithUV;
typedef MultisampleShaderConstants ShaderConstants;

namespace
{
gtf::scdr::ShaderProgram* createShaderProgram(
    gtf::scdr::GPUContext&    gc_,
    gtf::scdr::CommandBuffer& commandBuffer_,
    const char*               vertexShader_,
    const char*               fragmentShader)
{
    gtf::scdr::Shader*        vs = nullptr;
    gtf::scdr::Shader*        fs = nullptr;
    gtf::scdr::ShaderProgram* prog = nullptr;

    vs = gc_.createShader(gtf::scdr::Shader::Type::Vertex, vertexShader_, nullptr, 0, commandBuffer_);
    fs = gc_.createShader(gtf::scdr::Shader::Type::Fragment, fragmentShader, nullptr, 0, commandBuffer_);

    if (vs && fs)
    {
        const gtf::scdr::ShaderProgramSet shaders = { vs, fs };

        prog = gc_.createShaderProgram(shaders, commandBuffer_);
    }

    gc_.destroyShader(vs, commandBuffer_);
    gc_.destroyShader(fs, commandBuffer_);

    return prog;
}

void createPipelineStateObject(
    gtf::scdr::GPUContext&                    gc_,
    gtf::scdr::CommandBuffer&                 commandBuffer_,
    gtf::scdr::ShaderProgram*                 program_,
    const gtf::scdr::PipelineInputDescriptor* inputDesc_,
    const uint32_t                            inputDescCount_,
    gtf::scdr::PrimitiveTopology::Value       primitiveTopology_,
    gtf::scdr::PipelineStateObject*&          psoOut_)
{
    gtf::scdr::PipelineStateObjectDescriptor psoDesc;
    psoDesc.inputDescriptors = inputDesc_;
    psoDesc.inputDescriptorsCount = inputDescCount_;
    psoDesc.program = program_;
    psoDesc.primitiveTopology = primitiveTopology_;

    gtf::scdr::CreatePremultipliedAdditiveBlendStateDescriptor(psoDesc.blendStateDescriptor);

    gtf::scdr::CreateDefaultDepthStencilStateDescriptor(psoDesc.depthStencilStateDescriptor);
    gtf::scdr::CreateDefaultRasterizerStateDescriptor(psoDesc.rasterizerStateDescriptor);

    // Always enable the scissor state
    psoDesc.rasterizerStateDescriptor.scissorEnable = true;

    psoOut_ = gc_.createPipelineStateObject(psoDesc, commandBuffer_);
}

bool createPipelineStateObjects(
    gtf::scdr::GPUContext&           gc_,
    gtf::scdr::CommandBuffer&        commandBuffer_,
    gtf::scdr::ShaderProgram*        offscreenProgram_,
    gtf::scdr::ShaderProgram*        program_,
    gtf::scdr::ShaderProgram*        programWithAlphaMask_,
    MultisamplePipelineStateObjects& psos_)
{
    static const gtf::scdr::PipelineInputDescriptor offscreenInputDesc[] =
    {
        { "aPosition", gtf::scdr::VertexFormat::X32Y32_FLOAT, 0, 0, gtf::scdr::InputClassification::VertexData, 0 }
    };

    static const gtf::scdr::PipelineInputDescriptor defaultInputDesc[] =
    {
        { "aPosition", gtf::scdr::VertexFormat::X32Y32_FLOAT, 0, 0, gtf::scdr::InputClassification::VertexData, 0 },
        { "aTexCoord", gtf::scdr::VertexFormat::X32Y32_FLOAT, 0, 0, gtf::scdr::InputClassification::VertexData, 0 }
    };

    createPipelineStateObject(gc_,
        commandBuffer_,
        offscreenProgram_,
        offscreenInputDesc,
        ARRAY_SIZE(offscreenInputDesc),
        gtf::scdr::PrimitiveTopology::TriangleList,
        psos_.offscreenPSO);

    createPipelineStateObject(gc_,
        commandBuffer_,
        program_,
        defaultInputDesc,
        ARRAY_SIZE(defaultInputDesc),
        gtf::scdr::PrimitiveTopology::TriangleStrip,
        psos_.defaultPSO);

    createPipelineStateObject(gc_,
        commandBuffer_,
        programWithAlphaMask_,
        defaultInputDesc,
        ARRAY_SIZE(defaultInputDesc),
        gtf::scdr::PrimitiveTopology::TriangleStrip,
        psos_.defaultPSOWithAlphaMask);

    return psos_.offscreenPSO && psos_.defaultPSO && psos_.defaultPSOWithAlphaMask;
}

bool createShaderConstants(
    gtf::scdr::ShaderProgram* offscreenProgram_,
    gtf::scdr::ShaderProgram* program_,
    gtf::scdr::ShaderProgram* programWithAlphaMask_,
    gtf::scdr::GPUContext&    gc_,
    gtf::scdr::CommandBuffer& commandBuffer_,
    ShaderConstants&          shaderConstants)
{
    shaderConstants.offscreenPass.projMat = gc_.createShaderConstant("uProjMat", offscreenProgram_, commandBuffer_);
    shaderConstants.offscreenPass.modelMat = gc_.createShaderConstant("uModelMat", offscreenProgram_, commandBuffer_);
    shaderConstants.offscreenPass.color = gc_.createShaderConstant("uColor", offscreenProgram_, commandBuffer_);

    shaderConstants.defaultPass.projMat = gc_.createShaderConstant("uProjMat", program_, commandBuffer_);
    shaderConstants.defaultPass.modelMat = gc_.createShaderConstant("uModelMat", program_, commandBuffer_);
    shaderConstants.defaultPass.texture = gc_.createShaderConstant("uTexture", program_, commandBuffer_);

    shaderConstants.defaultPassWithAlphaMask.projMat = gc_.createShaderConstant("uProjMat", programWithAlphaMask_, commandBuffer_);
    shaderConstants.defaultPassWithAlphaMask.modelMat = gc_.createShaderConstant("uModelMat", programWithAlphaMask_, commandBuffer_);
    shaderConstants.defaultPassWithAlphaMask.texture = gc_.createShaderConstant("uTexture", programWithAlphaMask_, commandBuffer_);
    shaderConstants.defaultPassWithAlphaMask.object2Alpha = gc_.createShaderConstant("uObject2Alpha", programWithAlphaMask_, commandBuffer_);
    shaderConstants.defaultPassWithAlphaMask.textureAlphaMask = gc_.createShaderConstant("uTextureAlphaMask", programWithAlphaMask_, commandBuffer_);

    return (shaderConstants.offscreenPass.projMat && shaderConstants.offscreenPass.modelMat && shaderConstants.offscreenPass.color
        && shaderConstants.defaultPass.projMat && shaderConstants.defaultPass.modelMat && shaderConstants.defaultPass.texture && shaderConstants.defaultPassWithAlphaMask.projMat
        && shaderConstants.defaultPassWithAlphaMask.modelMat && shaderConstants.defaultPassWithAlphaMask.texture && shaderConstants.defaultPassWithAlphaMask.textureAlphaMask
        && shaderConstants.defaultPassWithAlphaMask.object2Alpha);
}

bool isFramebufferDirty(const gtf::scdr::FramebufferDescriptor& fbDesc_, const uint32_t sampleCount_, const uint32_t width_, const uint32_t height_)
{
    return (fbDesc_.width != width_ || fbDesc_.height != height_ || fbDesc_.samples != sampleCount_);
}

void updateCanvas(
    gtf::scdr::DataStore&     dataStore_,
    gtf::scdr::CommandBuffer& cmdBuffer_,
    const uint32_t            sampleCount_,
    const uint32_t            width_,
    const uint32_t            height_,
    gtf::scdr::Framebuffer*&  fbRender_,
    gtf::scdr::Framebuffer*&  fbResolve_)
{
    BIND_GPU_CONTEXT(gc, dataStore_);

    gc.destroyFramebuffer(fbRender_, cmdBuffer_);
    gc.destroyFramebuffer(fbResolve_, cmdBuffer_);

    const gtf::scdr::TextureFormat::Value renderTargetFormat = gtf::scdr::TextureFormat::R8G8B8A8;
    const gtf::scdr::TextureFormat::Value depthFormat = gtf::scdr::TextureFormat::Unknown;
    const gtf::scdr::SystemCapabilities& sysCaps = gc.getSystemCapabilities();

    if (sampleCount_ > 1 && sysCaps.multisampleResolveOnWrite)
    {
        const gtf::scdr::FramebufferDescriptor fbDesc =
            gtf::scdr::CFramebufferDescriptor(
                width_,
                height_,
                sampleCount_,
                gtf::scdr::FramebufferFlags::ColorBufferAsTexture | gtf::scdr::FramebufferFlags::MultiSampleResolveOnWrite,
                renderTargetFormat,
                depthFormat);

        fbRender_ = gc.createFramebuffer(fbDesc, cmdBuffer_);
        fbResolve_ = gc.createFramebuffer(fbRender_);
    }
    else if (sampleCount_ > 1 && sysCaps.blit)
    {
        //
        // MSAA offscreen blit source
        //
        const gtf::scdr::FramebufferDescriptor fbBlitSourceDesc = gtf::scdr::CFramebufferDescriptor(
            width_,
            height_,
            sampleCount_,
            0,
            renderTargetFormat,
            depthFormat);

        //
        // regular offscreen blit destination (MSAA resolve target)
        //
        const gtf::scdr::FramebufferDescriptor fbBlitTargetDesc = gtf::scdr::CFramebufferDescriptor(
            width_,
            height_,
            1,
            gtf::scdr::FramebufferFlags::ColorBufferAsTexture,
            renderTargetFormat,
            depthFormat);

        fbRender_ = gc.createFramebuffer(fbBlitSourceDesc, cmdBuffer_);
        fbResolve_ = gc.createFramebuffer(fbBlitTargetDesc, cmdBuffer_);
    }
    else
    {
        //
        // Fallback / regular non-MSAA rendering
        //
        const gtf::scdr::FramebufferDescriptor fbDesc = gtf::scdr::CFramebufferDescriptor(
            width_,
            height_,
            1,
            gtf::scdr::FramebufferFlags::ColorBufferAsTexture,
            renderTargetFormat,
            depthFormat);

        fbRender_ = gc.createFramebuffer(fbDesc, cmdBuffer_);
        fbResolve_ = gc.createFramebuffer(fbRender_);
    }
}

void generateParticles(
    const uint32_t gridWidth_,
    const uint32_t gridHeight_,
    const uint32_t cellLength_,
    const float    animation_,
    Vertex* const  vertices_)
{
    const float totalHeight = float(cellLength_ * gridHeight_);

    for (uint32_t col = 0; col <= gridWidth_; ++col)
    {
        for (uint32_t row = 0; row < gridHeight_; ++row)
        {
            uint32_t index = (col * gridHeight_ + row);

            const float moveUpOffset = col % 2 ? 0.2f * cellLength_ : 0.4f * cellLength_;
            const Vertex origin = { float(col * cellLength_), (animation_ + row) * cellLength_ + moveUpOffset };

            float scaleFactor = origin.y / totalHeight;
            const Vertex v1 = { origin.x, origin.y - (scaleFactor * cellLength_ / 2.0f) };
            const Vertex v2 = { origin.x + (scaleFactor * cellLength_ / 2.0f), origin.y };
            const Vertex v3 = { origin.x, origin.y + (scaleFactor * cellLength_ / 2.0f) };
            const Vertex v4 = { origin.x - (scaleFactor * cellLength_ / 2.0f), origin.y };

            index *= 6;
            vertices_[index] = v2;
            vertices_[index + 1] = v3;
            vertices_[index + 2] = v4;

            vertices_[index + 3] = v2;
            vertices_[index + 4] = v4;
            vertices_[index + 5] = v1;
        }
    }
}
}

const char* Renderer::ClassName = "Multisample";

gtf::scdr::IWidgetRenderer* Renderer::createInstance(
    const gtf::scene::TypeResolver& resolver_,
    gtf::scdr::DataStore&           dataStore_,
    gtf::scdr::CommandBuffer&       commandBuffer_,
    gtf::scdr::Allocator&           frameAllocator_,
    const void*)
{
    BIND_ALLOCATOR(allocator, dataStore_);

    gtf::scdr::IWidgetRenderer* const renderer = ALLOC_OBJECT(allocator, Renderer, resolver_, dataStore_.getSystemConfig().getGraphicsApi());

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

void Renderer::destroyInstance(
    gtf::scdr::IWidgetRenderer* renderer_,
    gtf::scdr::DataStore&       dataStore_,
    gtf::scdr::CommandBuffer&   commandBuffer_,
    gtf::scdr::Allocator&       frameAllocator_)
{
    if (renderer_)
    {
        renderer_->deinit(dataStore_, commandBuffer_, frameAllocator_);

        BIND_ALLOCATOR(allocator, dataStore_);

        return FREE_OBJECT(allocator, static_cast<Renderer*>(renderer_));
    }
}

Renderer::Renderer(const gtf::scene::TypeResolver& resolver_, gtf::scdr::GraphicsApi api_)
    : api(api_)
    , objects(NULL)
    , animatedProperty(0.0f)
{
    // Resolves the widget's default handles
    gtf::scdr::ResolvePropertiesCommon2D(resolver_, ClassName, properties.common2d);
    gtf::scdr::ResolveFeaturesCommon2D(resolver_, features);

    // Resolves the widget specific handles
    gtf::scene::TypeResolver::ContainerPropertyReference wpr[] =
    {
        { { ClassName, gtf::properties::Container::Type() }, { "fillColor", gtf::types::limits::uint16_max } },
        { { ClassName, gtf::properties::Container::Type() }, { "sampleCount", gtf::types::limits::uint16_max } },
        { { ClassName, gtf::properties::Container::Type() }, { "animatedProperty", gtf::types::limits::uint16_max } }
    };

    resolver_.resolveContainerProperties(wpr, ARRAY_SIZE(wpr));

    properties.fillColor      = wpr[0].property.identifier;
    properties.sampleCount    = wpr[1].property.identifier;
    properties.animatedProperty = wpr[2].property.identifier;

    memset(&graphicObjects, 0, sizeof(graphicObjects));
}

bool Renderer::init(const gtf::scene::TypeResolver&, gtf::scdr::DataStore& dataStore_, gtf::scdr::CommandBuffer& cmdBuffer_, gtf::scdr::Allocator&)
{
    BIND_GPU_CONTEXT(gc, dataStore_);

    //
    // Initializes all relevant graphics objects (shader programs,
    // pipeline states, resource bindings, resources)
    //
    gtf::scdr::ShaderProgram *offscreenProgram = nullptr, *program = nullptr, *programWithAlphaMask = nullptr;
    switch (api)
    {
    case gtf::scdr::GraphicsApi::OpenGLES20:
        offscreenProgram = createShaderProgram(gc, cmdBuffer_, OffscreenVertexShaderGLES20, OffscreenFragmentShaderGLES20);
        program = createShaderProgram(gc, cmdBuffer_, VertexShaderGLES20, FragmentShaderGLES20);
        programWithAlphaMask = createShaderProgram(gc, cmdBuffer_, VertexShaderWithAlphaMaskGLES20, FragmentShaderWithAlphaMaskGLES20);
        break;

    case gtf::scdr::GraphicsApi::OpenGLES3:
        offscreenProgram = createShaderProgram(gc, cmdBuffer_, OffscreenVertexShaderGLES3, OffscreenFragmentShaderGLES3);
        program = createShaderProgram(gc, cmdBuffer_, VertexShaderGLES3, FragmentShaderGLES3);
        programWithAlphaMask = createShaderProgram(gc, cmdBuffer_, VertexShaderWithAlphaMaskGLES3, FragmentShaderWithAlphaMaskGLES3);
        break;

    case gtf::scdr::GraphicsApi::Unknown:
    default:
        break;
    }

    bool success = (offscreenProgram != nullptr && program != nullptr && programWithAlphaMask != nullptr);
    if (!success)
    {
        return false;
    }

    success = createPipelineStateObjects(gc, cmdBuffer_, offscreenProgram, program, programWithAlphaMask, graphicObjects.psos);
    success = success && 
              createShaderConstants(offscreenProgram, program, programWithAlphaMask, gc, cmdBuffer_, graphicObjects.shaderConstants);

    //
    // We can safely destroy the shader program again after it has been used to
    // create pipeline state objects and resource binding tables.
    //
    gc.destroyShaderProgram(offscreenProgram, cmdBuffer_);
    gc.destroyShaderProgram(program, cmdBuffer_);
    gc.destroyShaderProgram(programWithAlphaMask, cmdBuffer_);
    offscreenProgram = program = programWithAlphaMask = nullptr;

    if (!success)
    {
        return false;
    }

    const gtf::scdr::BufferDescriptor triangleVBDesc = gtf::scdr::CBufferDescriptor(
        gtf::scdr::BufferUsage::Dynamic,
        sizeof(vertices));
    graphicObjects.vertexBuffer = gc.createVertexBuffer(triangleVBDesc, vertices, cmdBuffer_);

    static const VertexWithUV quadVertices[] =
    {
        { 0.0f, 0.0f, 0.0f, 0.0f },
        { 1.0f, 0.0f, 1.0f, 0.0f },
        { 0.0f, 1.0f, 0.0f, 1.0f },
        { 1.0f, 1.0f, 1.0f, 1.0f }
    };

    const gtf::scdr::BufferDescriptor quadVBDesc = gtf::scdr::CBufferDescriptor(
        gtf::scdr::BufferUsage::Immutable,
        sizeof(quadVertices));
    graphicObjects.compositingQuadVertexBuffer = gc.createVertexBuffer(quadVBDesc, quadVertices, cmdBuffer_);

    success = graphicObjects.vertexBuffer != nullptr && graphicObjects.compositingQuadVertexBuffer != nullptr;

    graphicObjects.sampler = gc.createSamplerState(
        gtf::scdr::CSamplerStateDescriptor(gtf::scdr::TextureFilterMode::Linear_MipNone),
        cmdBuffer_);
    success = success && graphicObjects.sampler != nullptr;

    return success;
}

void Renderer::deinit(gtf::scdr::DataStore& dataStore_, gtf::scdr::CommandBuffer& commandBuffer_, gtf::scdr::Allocator&)
{
    BIND_GPU_CONTEXT(gc, dataStore_);

    gc.destroyVertexBuffer(graphicObjects.vertexBuffer, commandBuffer_);
    gc.destroyVertexBuffer(graphicObjects.compositingQuadVertexBuffer, commandBuffer_);

    gc.destroyPipelineStateObject(graphicObjects.psos.offscreenPSO, commandBuffer_);
    gc.destroyPipelineStateObject(graphicObjects.psos.defaultPSO, commandBuffer_);
    gc.destroyPipelineStateObject(graphicObjects.psos.defaultPSOWithAlphaMask, commandBuffer_);

    gc.destroySamplerState(graphicObjects.sampler, commandBuffer_);

    gc.destroyShaderConstant(graphicObjects.shaderConstants.offscreenPass.projMat, commandBuffer_);
    gc.destroyShaderConstant(graphicObjects.shaderConstants.offscreenPass.modelMat, commandBuffer_);
    gc.destroyShaderConstant(graphicObjects.shaderConstants.offscreenPass.color, commandBuffer_);

    gc.destroyShaderConstant(graphicObjects.shaderConstants.defaultPass.projMat, commandBuffer_);
    gc.destroyShaderConstant(graphicObjects.shaderConstants.defaultPass.modelMat, commandBuffer_);
    gc.destroyShaderConstant(graphicObjects.shaderConstants.defaultPass.texture, commandBuffer_);

    gc.destroyShaderConstant(graphicObjects.shaderConstants.defaultPassWithAlphaMask.projMat, commandBuffer_);
    gc.destroyShaderConstant(graphicObjects.shaderConstants.defaultPassWithAlphaMask.modelMat, commandBuffer_);
    gc.destroyShaderConstant(graphicObjects.shaderConstants.defaultPassWithAlphaMask.texture, commandBuffer_);
    gc.destroyShaderConstant(graphicObjects.shaderConstants.defaultPassWithAlphaMask.object2Alpha, commandBuffer_);
    gc.destroyShaderConstant(graphicObjects.shaderConstants.defaultPassWithAlphaMask.textureAlphaMask, commandBuffer_);
    memset(&graphicObjects.shaderConstants, 0, sizeof(graphicObjects.shaderConstants));
}

bool Renderer::begin(gtf::scdr::DataStore&, gtf::scdr::CommandBuffer&, gtf::scdr::Allocator&)
{
    GTF_OSAL_ASSERT(objects == NULL);
    return true;
}

bool Renderer::traverse(
    gtf::properties::Container&              widget_,
    const gtf::scdr::RenderObjectStackFrame& parentStackFrame_,
    gtf::scdr::RenderObjectStackFrame&       objectStackFrame_,
    gtf::scdr::RenderObject&                 object_,
    gtf::scdr::TraverserContext&             tc_)
{
    MultisampleStackFrame& objectStack =
        static_cast<MultisampleStackFrame&>(objectStackFrame_);

    MultisampleRenderObject& object = static_cast<MultisampleRenderObject&>(object_);

    // 1. Load stack values
    gtf::scdr::RenderObjectStackFrame2DValues stackValues;
    gtf::scdr::GetRenderObjectStackFrameValues2D(parentStackFrame_, stackValues);

    // 2. Load default values
    if (!object.resolveObjectParams(stackValues))
    {
        return false;
    }

    // 3. Set up Multisampled framebuffer
    {
        int32_t sampleCount = 1;
        {
            int32_t sampleCountExp = 0;
            gtf::scdr::LoadValue(widget_, properties.sampleCount, sampleCountExp);
            sampleCount = 1 << sampleCountExp;
        }

        if (!object.fbRender || !object.fbResolve
            || isFramebufferDirty(object.fbRender->getDescriptor(), sampleCount, object.propertyValues.width, object.propertyValues.height))
        {
            updateCanvas(tc_.getDataStore(),
                tc_.getCommandBuffer(),
                sampleCount,
                object.propertyValues.width,
                object.propertyValues.height,
                object.fbRender,
                object.fbResolve);

            if (!object.fbRender || !object.fbResolve)
            {
                GTF_WARNING(GTF_RENDERER_PLUGIN_Multisample, "Failed to create multisampled framebuffers.");
                return false;
            }
        }

        gtf::scdr::LoadValue(widget_, properties.animatedProperty, animatedProperty);
    }

    // 4. Update transforms and configure the draw object
    {
        const uint32_t                   drawId = gtf::scdr::GetWidgetDepth(stackValues);
        const gtf::utils::math::float4x4 unit2Framebuffer = object.transforms.widget2Framebuffer * 
                                                            gtf::utils::math::float4x4::Scaling2D(object.width, object.height) *
                                                            gtf::utils::math::float4x4::Translation(0.f, 0.f, static_cast<float>(drawId));
        object.drawId = drawId;
        object.framebuffer = stackValues.framebuffer;
        object.framebufferId = stackValues.framebufferId;
        object.viewport = *stackValues.viewport;

        object.uniforms.defaultPass.projMat = *stackValues.viewProjMat;
        object.uniforms.defaultPass.quadMat = unit2Framebuffer;

        gtf::scdr::CalculateViewProjectionMatrix2D(
            object.propertyValues.width,
            object.propertyValues.height,
            1,
            api,
            object.uniforms.offscreenPass.projMat);
        const float gridSideLength = gridSideCellCount * cellLength;
        object.uniforms.offscreenPass.modelMat = gtf::utils::math::float4x4::Scaling2D(object.width / gridSideLength, object.height / gridSideLength);

        gtf::scdr::LoadValue(widget_, properties.fillColor, object.uniforms.offscreenPass.fillColor);
        object.uniforms.offscreenPass.fillColor *= object.coloration;

        object.alphaMask = stackValues.alphaMask;
        object.uniforms.object2Alpha = object.transforms.widget2AlphaMask * gtf::utils::math::float4x4::Scaling2D(object.width, object.height);
    }

    // 5. Propagate relevant data to child widgets
    objectStack.store(object.transforms, stackValues, object.coloration, object.scissor);

    // 6. Append to render list
    if (object.propertyValues.width > 0 && object.propertyValues.height > 0)
    {
        object.next = objects;
        this->objects = &object;
    }

    return true;
}

gtf::scdr::RenderObject* Renderer::createRenderObject(
    gtf::properties::Container& widget_,
    gtf::scdr::DataStore&       dataStore_,
    gtf::scdr::CommandBuffer&,
    gtf::scdr::Allocator&)
{
    BIND_ALLOCATOR(allocator, dataStore_);

    return ALLOC_OBJECT(allocator, MultisampleRenderObject, widget_, features, properties.common2d);
}

void Renderer::destroyRenderObject(
    gtf::scdr::RenderObject*  object_,
    gtf::scdr::DataStore&     dataStore_,
    gtf::scdr::CommandBuffer& commandBuffer_,
    gtf::scdr::Allocator&)
{
    BIND_ALLOCATOR(allocator, dataStore_);
    BIND_GPU_CONTEXT(gc, dataStore_);

    MultisampleRenderObject* object = static_cast<MultisampleRenderObject*>(object_);

    gc.destroyFramebuffer(object->fbRender, commandBuffer_);
    gc.destroyFramebuffer(object->fbResolve, commandBuffer_);

    object->fbRender = nullptr;
    object->fbResolve = nullptr;

    FREE_OBJECT(allocator, object);
}

bool Renderer::update(
    gtf::scdr::ResourceLoader&,
    gtf::scdr::DataStore& dataStore_,
    gtf::scdr::CommandBuffer& cmdBuffer_,
    gtf::scdr::Allocator&)
{
    generateParticles(gridSideCellCount, gridSideCellCount, cellLength, animatedProperty, vertices);
    BIND_GPU_CONTEXT(gc, dataStore_);
    const uint32_t sizeInBytes = graphicObjects.vertexBuffer->getDescriptor().sizeInBytes;
    gc.updateVertexBuffer(graphicObjects.vertexBuffer, vertices, sizeInBytes, cmdBuffer_);

    return true;
}

bool Renderer::render(gtf::scdr::DataStore&, gtf::scdr::CommandBuffer& commandBuffer_, gtf::scdr::Allocator&)
{
    for (MultisampleRenderObject* object = objects; object; object = object->next)
    {
        const bool withAlphaMask = object->alphaMask != NULL;
        const gtf::scdr::ScheduleInfo info = gtf::scdr::CScheduleInfo(
            withAlphaMask ? graphicObjects.psos.defaultPSOWithAlphaMask : graphicObjects.psos.defaultPSO,
            object->framebufferId,
            object->drawId,
            gtf::scdr::ScheduleInfo::Transparent);

        MultisampleCommand* const drawCmd = MultisampleCommand::create(commandBuffer_, &graphicObjects, object);
        if (drawCmd)
        {
            commandBuffer_.enqueueRender(info, drawCmd);
        }
    }
    return true;
}

bool Renderer::end(gtf::scdr::DataStore&, gtf::scdr::CommandBuffer&, gtf::scdr::Allocator&)
{
    objects = NULL;
    return true;
}
