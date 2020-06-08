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
#include <gtf/scdr/RenderObjectBase.h>
#include <gtf/scdr/command/Command.h>
#include <gtf/scdr/gpu/GPUExecutionContext.h>
#include <gtf/scdr/gpu/Scissor.h>
#include <gtf/scdr/SDK.h>

#include <gtf/scdr/gpu/BlendStateDescriptor.h>
#include <gtf/scdr/gpu/BufferDescriptor.h>
#include <gtf/scdr/gpu/DepthStencilStateDescriptor.h>
#include <gtf/scdr/gpu/DrawStateDescriptor.h>
#include <gtf/scdr/gpu/RasterizerStateDescriptor.h>
#include <gtf/scdr/gpu/ProgramDescriptor.h>
#include <gtf/scdr/cache/ShaderCache.h>

#include <gtf/types/Macros.h>

#include "GLES20Shader.h"
#include "GLES3Shader.h"
#include "TriangleRenderer.h"

struct TriangleVertex
{
    float x;
    float y;
};

struct TriangleUniforms
{
    gtf::utils::math::float4x4 projMat;
    gtf::utils::math::float4x4 triMat;
    gtf::scdr::Color           fillColor;
};

struct AlphaMaskUniforms
{
    gtf::utils::math::float4x4 object2Alpha;
};

struct TriangleDrawContext
{
    TriangleUniforms*               uniforms;
    AlphaMaskUniforms*              uniformsAlphaMask;
    gtf::scdr::PipelineStateObject* pipelineStateObject;
    TriangleShaderConstants*        shaderConstants;
    gtf::scdr::VertexBuffer*        vertexBuffer;
    gtf::scdr::Framebuffer*         framebuffer;
    const gtf::scdr::Viewport*      viewport;
    const gtf::scdr::Scissor*       scissor;
    gtf::scdr::Texture2D*           texture;
    gtf::scdr::SamplerState*        sampler;
};

class DrawTriangleCommand : public gtf::scdr::Command
{
public:
    DrawTriangleCommand()
        : Command()
    {
        memset(&drawContext, 0, sizeof(drawContext));
    }
    virtual ~DrawTriangleCommand()
    {
    }

    static DrawTriangleCommand* create(gtf::scdr::CommandBuffer& commandBuffer_)
    {
        gtf::scdr::Allocator& commandAllocator = commandBuffer_.getCommandAllocator();

        return ALLOC_OBJECT(commandAllocator, DrawTriangleCommand);
    }

    uint32_t execute(gtf::scdr::GPUExecutionContext& executionContext_, gtf::scdr::Allocator&)
    {
        const uint32_t           vbstride[] = { sizeof(TriangleVertex) };
        const uint32_t           vboffset[] = { 0 };
        gtf::scdr::VertexBuffer* vb[]       = { drawContext.vertexBuffer };

        const gtf::scdr::DrawStateDescriptor drawStateDesc =
            {
                drawContext.framebuffer,
                drawContext.pipelineStateObject,
                *drawContext.viewport,
                *drawContext.scissor,
                0
            };

        executionContext_.setDrawState(drawStateDesc);

        executionContext_.setShaderConstant(*drawContext.shaderConstants->projMat, drawContext.uniforms->projMat.values, 16);
        executionContext_.setShaderConstant(*drawContext.shaderConstants->modelMat, drawContext.uniforms->triMat.values, 16);
        executionContext_.setShaderConstant(*drawContext.shaderConstants->color, &drawContext.uniforms->fillColor.r, 4);

        if (drawContext.shaderConstants->textureAlphaMask != NULL)
        {
            executionContext_.setTexture(*drawContext.shaderConstants->textureAlphaMask, 0, drawContext.texture, drawContext.sampler);
            executionContext_.setShaderConstant(*drawContext.shaderConstants->object2Alpha, drawContext.uniformsAlphaMask->object2Alpha.values, 16);
        }

        executionContext_.setVertexBuffers(vb, vbstride, vboffset, ARRAY_SIZE(vb));
        executionContext_.draw(3, 0);

        return gtf::scdr::CommandErrorCode::Success;
    }

public:
    TriangleDrawContext drawContext;
};

class TriangleRenderObjectStackFrame : public gtf::scdr::RenderObjectStackFrame2D
{
public:
    void load(gtf::scdr::RenderObjectStackFrame2DValues& valuesOut_) const
    {
        valuesOut_.coloration         = &coloration;
        valuesOut_.framebuffer        = framebuffer;
        valuesOut_.framebufferId      = framebufferId;
        valuesOut_.widget2Framebuffer = &widget2Framebuffer;
        valuesOut_.framebuffer2Screen = framebuffer2Screen;
        valuesOut_.scissor            = &scissor;
        valuesOut_.viewport           = viewport;
        valuesOut_.viewProjMat        = viewProjMat;
        valuesOut_.depth              = depth;
        valuesOut_.alphaMask          = alphaMask;
        valuesOut_.widget2AlphaMask   = &widget2AlphaMask;
    }

    void store(const gtf::scdr::WidgetTransformations&   transforms_,
        const gtf::scdr::RenderObjectStackFrame2DValues& parentValues_,
        const gtf::scdr::Color&                          coloration_,
        const gtf::scdr::Scissor&                        scissor_)
    {
        coloration         = coloration_;
        framebuffer        = parentValues_.framebuffer;
        framebufferId      = parentValues_.framebufferId;
        widget2Framebuffer = transforms_.widget2StackFrame;
        framebuffer2Screen = parentValues_.framebuffer2Screen;
        scissor            = scissor_;
        viewport           = parentValues_.viewport;
        viewProjMat        = parentValues_.viewProjMat;
        depth              = parentValues_.depth;
        alphaMask          = parentValues_.alphaMask;
        widget2AlphaMask   = transforms_.widget2AlphaMask;
    }

private:
    const gtf::utils::math::float4x4* viewProjMat;
    const gtf::utils::math::float4x4* framebuffer2Screen;
    const gtf::scdr::Viewport*        viewport;

    gtf::utils::math::float4x4  widget2Framebuffer;
    gtf::utils::math::float4x4  widget2AlphaMask;
    gtf::scdr::Scissor          scissor;
    gtf::scdr::Color            coloration;
    gtf::scdr::Framebuffer*     framebuffer;
    const gtf::scdr::AlphaMask* alphaMask;
    uint32_t                    framebufferId;
    uint32_t*                   depth;
};

class TriangleRenderObject
    : public gtf::scdr::RenderObjectBase,
      public gtf::scdr::TouchObject
{
public:
    TriangleRenderObject(gtf::properties::Container& widget_, const gtf::scdr::WidgetFeaturesCommon2D& featureHandles_, const gtf::scdr::WidgetPropertiesCommon2D& propertyHandles_)
        : RenderObjectBase(widget_, featureHandles_, propertyHandles_)
        , touchBehavior(gtf::scdr::TouchBehavior::WholeArea)
        , alphaMask(NULL)
    {
    }

public: // sdk::RenderObject interface
    virtual gtf::scdr::RenderObjectStackFrame* createStackFrame(gtf::scdr::RenderObjectStackFrame&, gtf::scdr::Allocator& allocator_)
    {
        return ALLOC_OBJECT(allocator_, TriangleRenderObjectStackFrame);
    }

    virtual void destroyStackFrame(gtf::scdr::RenderObjectStackFrame* frame_, gtf::scdr::Allocator& allocator_)
    {
        FREE_OBJECT(allocator_, static_cast<TriangleRenderObjectStackFrame*>(frame_));
    }

    virtual const gtf::scdr::TouchObject* getTouchObject()
    {
        if (featureValues.mask & gtf::scdr::WidgetFeaturesCommon2DValues::Touch && width > 0.f && height > 0.f)
        {
            return this;
        }
        return NULL;
    }

public: // sdk::TouchObject interface
    virtual void getBoundingRectangle(gtf::scdr::Rectangle<int16_t>& rectOut_) const
    {
        gtf::scdr::TransformCoordinate coord[] =
            {
                { 0.f, 0.f },
                { 0.f, 1.f },
                { 1.f, 0.f },
                { 1.f, 1.f }
            };

        transformCoordinates(gtf::scdr::TransformMode::Unit2Screen, ARRAY_SIZE(coord), coord);

        rectOut_.xmin = static_cast<int16_t>(MIN(coord[0].x, MIN(coord[1].x, MIN(coord[2].x, coord[3].x))));
        rectOut_.xmax = static_cast<int16_t>(MAX(coord[0].x, MAX(coord[1].x, MAX(coord[2].x, coord[3].x))));
        rectOut_.ymin = static_cast<int16_t>(MIN(coord[0].y, MIN(coord[1].y, MIN(coord[2].y, coord[3].y))));
        rectOut_.ymax = static_cast<int16_t>(MAX(coord[0].y, MAX(coord[1].y, MAX(coord[2].y, coord[3].y))));
    }

    virtual bool intersect(const gtf::scdr::ScreenPosition& pos_) const
    {
        gtf::scdr::TransformCoordinate coord =
            {
                static_cast<float>(pos_.x), static_cast<float>(pos_.y)
            };

        if (transformCoordinates(gtf::scdr::TransformMode::Screen2Unit, 1, &coord))
        {
            if (touchBehavior == gtf::scdr::TouchBehavior::WholeArea)
            {
                return 0.f <= coord.x && coord.x <= 1.f && 0.f <= coord.y && coord.y <= 1.f;
            }
            else
            {
                static const float p1x = 0.f, p1y = 1.f;
                static const float p2x = 1.f, p2y = 1.f;
                static const float p3x = .5f, p3y = 0.f;

                const bool a = ((coord.x - p2x) * (p1y - p2y) - (p1x - p2x) * (coord.y - p2y)) < 0.f;
                const bool b = ((coord.x - p3x) * (p2y - p3y) - (p2x - p3x) * (coord.y - p3y)) < 0.f;
                const bool c = ((coord.x - p1x) * (p3y - p1y) - (p3x - p1x) * (coord.y - p1y)) < 0.f;

                return (a == b) && (b == c);
            }
        }
        return false;
    }

public:
    TriangleRenderObject* next;

    gtf::scdr::Viewport             viewport;
    gtf::scdr::Framebuffer*         framebuffer;
    gtf::scdr::TouchBehavior::Value touchBehavior;
    uint32_t                        framebufferId;
    const gtf::scdr::AlphaMask*     alphaMask;

    TriangleUniforms  triangleParams;
    AlphaMaskUniforms alphaMaskParams;
    uint32_t          drawId;
};

const char* TriangleRenderer::ClassName = "Triangle";

TriangleRenderer::TriangleRenderer(TriangleRenderer&)
{
}

gtf::scdr::IWidgetRenderer* TriangleRenderer::createInstance(
    const gtf::scene::TypeResolver& resolver_,
    gtf::scdr::DataStore&           dataStore_,
    gtf::scdr::CommandBuffer&       commandBuffer_,
    gtf::scdr::Allocator&           frameAllocator_,
    const void*)
{
    BIND_ALLOCATOR(allocator, dataStore_);

    gtf::scdr::IWidgetRenderer* const renderer = ALLOC_OBJECT(allocator, TriangleRenderer, resolver_, dataStore_.getSystemConfig().getGraphicsApi());

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

void TriangleRenderer::destroyInstance(
    gtf::scdr::IWidgetRenderer* renderer_,
    gtf::scdr::DataStore&       dataStore_,
    gtf::scdr::CommandBuffer&   commandBuffer_,
    gtf::scdr::Allocator&       frameAllocator_)
{
    if (renderer_)
    {
        renderer_->deinit(dataStore_, commandBuffer_, frameAllocator_);

        BIND_ALLOCATOR(allocator, dataStore_);

        return FREE_OBJECT(allocator, static_cast<TriangleRenderer*>(renderer_));
    }
}

TriangleRenderer::TriangleRenderer(const gtf::scene::TypeResolver& resolver_, gtf::scdr::GraphicsApi api_)
    : vertexBuffer(NULL)
    , vertexBufferWithAlphaMask(NULL)
    , objects(NULL)
    , api(api_)
{
    pipelineStateObject[RenderModeOpaque] = pipelineStateObject[RenderModeTransparent] = NULL;
    pipelineStateObjectWithAlphaMask[RenderModeOpaque] = pipelineStateObjectWithAlphaMask[RenderModeTransparent] = NULL;

    // Resolves the widget's default handles
    gtf::scdr::ResolvePropertiesCommon2D(resolver_, ClassName, properties.common2d);
    gtf::scdr::ResolveFeaturesCommon2D(resolver_, features);

    // Resolves the widget specific handles
    gtf::scene::TypeResolver::ContainerPropertyReference wpr[] =
        {
            { { ClassName, gtf::properties::Container::Type() }, { "fillColor", gtf::types::limits::uint16_max } }
        };

    resolver_.resolveContainerProperties(wpr, ARRAY_SIZE(wpr));

    properties.fillColor = wpr[0].property.identifier;

    memset(&shaderConstants, 0, sizeof(shaderConstants));
    memset(&shaderConstantsWithAlphaMask, 0, sizeof(shaderConstantsWithAlphaMask));
}

TriangleRenderer::~TriangleRenderer()
{
}

gtf::scdr::ShaderProgram* TriangleRenderer::createShaderProgram(gtf::scdr::DataStore& dataStore_, gtf::scdr::CommandBuffer& commandBuffer_) const
{
    if ((api != gtf::scdr::GraphicsApi::OpenGLES20) && (api != gtf::scdr::GraphicsApi::OpenGLES3))
    {
        return NULL; // No shaders available for API
    }

    BIND_GPU_CONTEXT(gc, dataStore_);
    BIND_INTERFACE(cache, gtf::scdr::ShaderCache, dataStore_);
    gtf::scdr::ProgramDescriptor desc;
    if (api == gtf::scdr::GraphicsApi::OpenGLES20)
    {
        desc.vs.source       = VertexShaderGLES20;
        desc.vs.sourceLength = static_cast<uint32_t>(strlen(VertexShaderGLES20));
        desc.ps.source       = FragmentShaderGLES20;
        desc.ps.sourceLength = static_cast<uint32_t>(strlen(FragmentShaderGLES20));
    }
    else if (api == gtf::scdr::GraphicsApi::OpenGLES3)
    {
        desc.vs.source       = VertexShaderGLES3;
        desc.vs.sourceLength = static_cast<uint32_t>(strlen(VertexShaderGLES3));
        desc.ps.source       = FragmentShaderGLES3;
        desc.ps.sourceLength = static_cast<uint32_t>(strlen(FragmentShaderGLES3));
    }

    desc.vs.macros     = NULL;
    desc.vs.macroCount = 0;
    desc.ps.macros     = NULL;
    desc.ps.macroCount = 0;

    gtf::scdr::ShaderProgram* prog = cache->createProgram(desc, gc, commandBuffer_);

    return prog;
}

gtf::scdr::ShaderProgram* TriangleRenderer::createShaderProgramWithAlphaMask(
    gtf::scdr::DataStore&     dataStore_,
    gtf::scdr::CommandBuffer& commandBuffer_) const
{
    if ((api != gtf::scdr::GraphicsApi::OpenGLES20) && (api != gtf::scdr::GraphicsApi::OpenGLES3))
    {
        return NULL; // No shaders available for API
    }

    BIND_GPU_CONTEXT(gc, dataStore_);
    BIND_INTERFACE(cache, gtf::scdr::ShaderCache, dataStore_);
    gtf::scdr::ProgramDescriptor desc;
    if (api == gtf::scdr::GraphicsApi::OpenGLES20)
    {
        desc.vs.source       = VertexShaderWithAlphaMaskGLES20;
        desc.vs.sourceLength = static_cast<uint32_t>(strlen(VertexShaderWithAlphaMaskGLES20));
        desc.ps.source       = FragmentShaderWithAlphaMaskGLES20;
        desc.ps.sourceLength = static_cast<uint32_t>(strlen(FragmentShaderWithAlphaMaskGLES20));
    }
    else if (api == gtf::scdr::GraphicsApi::OpenGLES3)
    {
        desc.vs.source       = VertexShaderWithAlphaMaskGLES3;
        desc.vs.sourceLength = static_cast<uint32_t>(strlen(VertexShaderWithAlphaMaskGLES3));
        desc.ps.source       = FragmentShaderWithAlphaMaskGLES3;
        desc.ps.sourceLength = static_cast<uint32_t>(strlen(FragmentShaderWithAlphaMaskGLES3));
    }

    desc.vs.macros     = NULL;
    desc.vs.macroCount = 0;
    desc.ps.macros     = NULL;
    desc.ps.macroCount = 0;

    gtf::scdr::ShaderProgram* prog = cache->createProgram(desc, gc, commandBuffer_);

    return prog;
}

bool TriangleRenderer::createPipelineStateObject(
    RenderMode                       mode_,
    gtf::scdr::ShaderProgram*        program_,
    gtf::scdr::GPUContext&           gc_,
    gtf::scdr::CommandBuffer&        commandBuffer_,
    gtf::scdr::PipelineStateObject*& psoOut_) const
{
    static const gtf::scdr::PipelineInputDescriptor inputDesc[] =
        {
            { "aPosition", gtf::scdr::VertexFormat::X32Y32_FLOAT, 0, 0, gtf::scdr::InputClassification::VertexData, 0 }
        };

    gtf::scdr::PipelineStateObjectDescriptor psoDesc;

    psoDesc.inputDescriptors      = inputDesc;
    psoDesc.inputDescriptorsCount = ARRAY_SIZE(inputDesc);
    psoDesc.program               = program_;
    psoDesc.primitiveTopology     = gtf::scdr::PrimitiveTopology::TriangleStrip;

    if (mode_ == RenderModeTransparent)
    {
        gtf::scdr::CreateAdditiveBlendStateDescriptor(psoDesc.blendStateDescriptor);
    }
    else
    {
        gtf::scdr::CreateOpaqueBlendStateDescriptor(psoDesc.blendStateDescriptor);
    }

    gtf::scdr::CreateDefaultDepthStencilStateDescriptor(psoDesc.depthStencilStateDescriptor);
    gtf::scdr::CreateDefaultRasterizerStateDescriptor(psoDesc.rasterizerStateDescriptor);

    // Always enable the scissor state
    psoDesc.rasterizerStateDescriptor.scissorEnable = true;

    psoOut_ = gc_.createPipelineStateObject(psoDesc, commandBuffer_);
    return (psoOut_ != NULL);
}

static bool CreateShaderConstants(
    gtf::scdr::ShaderProgram* program_,
    gtf::scdr::GPUContext&    gc_,
    gtf::scdr::CommandBuffer& commandBuffer_,
    bool                      withAlphaMask,
    TriangleShaderConstants&  shaderConstants)
{
    shaderConstants.projMat  = gc_.createShaderConstant("uProjMat", program_, commandBuffer_);
    shaderConstants.modelMat = gc_.createShaderConstant("uModelMat", program_, commandBuffer_);
    shaderConstants.color    = gc_.createShaderConstant("uColor", program_, commandBuffer_);
    if (withAlphaMask)
    {
        shaderConstants.object2Alpha     = gc_.createShaderConstant("uObject2Alpha", program_, commandBuffer_);
        shaderConstants.textureAlphaMask = gc_.createShaderConstant("uTextureAlphaMask", program_, commandBuffer_);
    }
    return shaderConstants.projMat != NULL && shaderConstants.modelMat != NULL && shaderConstants.color != NULL && (!withAlphaMask || (shaderConstants.object2Alpha != NULL && shaderConstants.textureAlphaMask != NULL));
}

static void DestroyShaderConstants(
    TriangleShaderConstants&  shaderConstants,
    gtf::scdr::GPUContext&    gc_,
    gtf::scdr::CommandBuffer& commandBuffer_)
{
    gc_.destroyShaderConstant(shaderConstants.projMat, commandBuffer_);
    gc_.destroyShaderConstant(shaderConstants.modelMat, commandBuffer_);
    gc_.destroyShaderConstant(shaderConstants.color, commandBuffer_);
    gc_.destroyShaderConstant(shaderConstants.object2Alpha, commandBuffer_);
    gc_.destroyShaderConstant(shaderConstants.textureAlphaMask, commandBuffer_);
    memset(&shaderConstants, 0, sizeof(shaderConstants));
}

bool TriangleRenderer::createVertexBuffer(
    gtf::scdr::GPUContext&    gc_,
    gtf::scdr::CommandBuffer& commandBuffer_,
    gtf::scdr::VertexBuffer*& vertexBufferOut_) const
{
    static const TriangleVertex vertices[] =
        {
            { 0.0f, 1.0f },
            { 1.0f, 1.0f },
            { 0.5f, 0.0f }
        };

    const gtf::scdr::BufferDescriptor vbdesc = gtf::scdr::CBufferDescriptor(
        gtf::scdr::BufferUsage::Immutable,
        sizeof(vertices));

    vertexBufferOut_ = gc_.createVertexBuffer(vbdesc, vertices, commandBuffer_);
    return vertexBufferOut_ != NULL;
}

bool TriangleRenderer::init(const gtf::scene::TypeResolver&, gtf::scdr::DataStore& dataStore_, gtf::scdr::CommandBuffer& commandBuffer_, gtf::scdr::Allocator&)
{
    BIND_GPU_CONTEXT(gc, dataStore_);
    BIND_INTERFACE(cache, gtf::scdr::ShaderCache, dataStore_);
    bool success = true;
    //
    // Initializes all relevant graphics objects (shader programs,
    // pipeline states, resource bindings, resources)
    //
    //
    gtf::scdr::ShaderProgram* const program = createShaderProgram(dataStore_, commandBuffer_);
    if (program)
    {
        success = createPipelineStateObject(RenderModeOpaque, program, gc, commandBuffer_, pipelineStateObject[RenderModeOpaque]);
        success = success ? createPipelineStateObject(RenderModeTransparent, program, gc, commandBuffer_, pipelineStateObject[RenderModeTransparent]) : false;
        success = success ? CreateShaderConstants(program, gc, commandBuffer_, false, shaderConstants) : false;

        //
        // We can safely destroy the shader program again after it has been used to
        // create pipeline state objects and resource binding tables.
        //
        cache->destroyProgram(program, gc, commandBuffer_);
    }
    else
    {
        return false;
    }

    gtf::scdr::ShaderProgram* const programWithAlphaMask = createShaderProgramWithAlphaMask(dataStore_, commandBuffer_);
    if (programWithAlphaMask)
    {
        success = createPipelineStateObject(RenderModeOpaque, programWithAlphaMask, gc, commandBuffer_, pipelineStateObjectWithAlphaMask[RenderModeOpaque]);
        success = success ? createPipelineStateObject(RenderModeTransparent, programWithAlphaMask, gc, commandBuffer_, pipelineStateObjectWithAlphaMask[RenderModeTransparent]) : false;
        success = success ? CreateShaderConstants(programWithAlphaMask, gc, commandBuffer_, true, shaderConstantsWithAlphaMask) : false;

        //
        // We can safely destroy the shader program again after it has been used to
        // create pipeline state objects and resource binding tables.
        //
        cache->destroyProgram(programWithAlphaMask, gc, commandBuffer_);
    }
    else
    {
        return false;
    }

    success = success ? createVertexBuffer(gc, commandBuffer_, vertexBuffer) : false;
    success = success ? createVertexBuffer(gc, commandBuffer_, vertexBufferWithAlphaMask) : false;

    return success;
}

void TriangleRenderer::deinit(gtf::scdr::DataStore& dataStore_, gtf::scdr::CommandBuffer& commandBuffer_, gtf::scdr::Allocator&)
{
    BIND_GPU_CONTEXT(gc, dataStore_);

    //
    // Free's all graphics resources
    //
    gc.destroyVertexBuffer(vertexBufferWithAlphaMask, commandBuffer_);
    gc.destroyVertexBuffer(vertexBuffer, commandBuffer_);
    gc.destroyPipelineStateObject(pipelineStateObjectWithAlphaMask[RenderModeOpaque], commandBuffer_);
    gc.destroyPipelineStateObject(pipelineStateObjectWithAlphaMask[RenderModeTransparent], commandBuffer_);
    gc.destroyPipelineStateObject(pipelineStateObject[RenderModeOpaque], commandBuffer_);
    gc.destroyPipelineStateObject(pipelineStateObject[RenderModeTransparent], commandBuffer_);
    DestroyShaderConstants(shaderConstantsWithAlphaMask, gc, commandBuffer_);
    DestroyShaderConstants(shaderConstants, gc, commandBuffer_);

    vertexBufferWithAlphaMask                               = NULL;
    vertexBuffer                                            = NULL;
    pipelineStateObject[RenderModeOpaque]                   = NULL;
    pipelineStateObject[RenderModeTransparent]              = NULL;
    pipelineStateObjectWithAlphaMask[RenderModeOpaque]      = NULL;
    pipelineStateObjectWithAlphaMask[RenderModeTransparent] = NULL;
}

bool TriangleRenderer::begin(gtf::scdr::DataStore&, gtf::scdr::CommandBuffer&, gtf::scdr::Allocator&)
{
    GTF_OSAL_ASSERT(objects == NULL);
    return true;
}

gtf::scdr::Color TriangleRenderer::getFillColor(gtf::properties::Container& widget_) const
{
    gtf::scdr::Color fillColor;
    gtf::scdr::LoadValue(widget_, properties.fillColor, fillColor);
    return fillColor;
}

bool TriangleRenderer::traverse(
    gtf::properties::Container&              widget_,
    const gtf::scdr::RenderObjectStackFrame& parentStackFrame_,
    gtf::scdr::RenderObjectStackFrame&       objectStackFrame_,
    gtf::scdr::RenderObject&                 object_,
    gtf::scdr::TraverserContext&             traverserContext_)
{
    GTF_UNUSED_PARAM(traverserContext_);

    TriangleRenderObjectStackFrame& objectStack =
        static_cast<TriangleRenderObjectStackFrame&>(objectStackFrame_);

    TriangleRenderObject& object = static_cast<TriangleRenderObject&>(object_);

    // 1. Load stack values
    gtf::scdr::RenderObjectStackFrame2DValues stackValues;
    gtf::scdr::GetRenderObjectStackFrameValues2D(parentStackFrame_, stackValues);

    // 2. Load the default values
    if (!object.resolveObjectParams(stackValues))
    {
        return false;
    }

    // 3. Update transforms
    const uint32_t                   drawId           = gtf::scdr::GetWidgetDepth(stackValues);
    const gtf::utils::math::float4x4 unit2Framebuffer = object.transforms.widget2Framebuffer * gtf::utils::math::float4x4::Scaling2D(object.width, object.height) * gtf::utils::math::float4x4::Translation(0.f, 0.f, static_cast<float>(drawId));

    // 5. Configure the draw object
    object.touchBehavior = object.featureValues.touchBehavior;
    object.drawId        = drawId;
    object.framebuffer   = stackValues.framebuffer;
    object.framebufferId = stackValues.framebufferId;
    object.viewport      = *stackValues.viewport;
    object.alphaMask     = stackValues.alphaMask;

    object.triangleParams.fillColor     = getFillColor(widget_) * object.coloration;
    object.triangleParams.projMat       = *stackValues.viewProjMat;
    object.triangleParams.triMat        = unit2Framebuffer;
    object.alphaMaskParams.object2Alpha = object.transforms.widget2AlphaMask * gtf::utils::math::float4x4::Scaling2D(object.width, object.height);

    // 6. Propagate relevant data to child widgets
    objectStack.store(object.transforms, stackValues, object.coloration, object.scissor);

    // 7. Append to render list
    if (object.propertyValues.width > 0 && object.propertyValues.height > 0)
    {
        object.next   = objects;
        this->objects = &object;
    }

    return true;
}

gtf::scdr::RenderObject* TriangleRenderer::createRenderObject(
    gtf::properties::Container& widget_,
    gtf::scdr::DataStore&       dataStore_,
    gtf::scdr::CommandBuffer&,
    gtf::scdr::Allocator&)
{
    BIND_ALLOCATOR(allocator, dataStore_);

    return ALLOC_OBJECT(allocator, TriangleRenderObject, widget_, features, properties.common2d);
}

void TriangleRenderer::destroyRenderObject(
    gtf::scdr::RenderObject* object_,
    gtf::scdr::DataStore&    dataStore_,
    gtf::scdr::CommandBuffer&,
    gtf::scdr::Allocator&)
{
    BIND_ALLOCATOR(allocator, dataStore_);

    FREE_OBJECT(allocator, static_cast<TriangleRenderObject*>(object_));
}

bool TriangleRenderer::update(
    gtf::scdr::ResourceLoader&,
    gtf::scdr::DataStore&,
    gtf::scdr::CommandBuffer&,
    gtf::scdr::Allocator&)
{
    return true;
}

bool TriangleRenderer::render(gtf::scdr::DataStore&, gtf::scdr::CommandBuffer& commandBuffer_, gtf::scdr::Allocator&)
{
    for (TriangleRenderObject* object = objects; object; object = object->next)
    {
        const bool withAlphaMask = object->alphaMask != NULL;

        const RenderMode renderMode = (object->triangleParams.fillColor.a == 1.f && object->alphaMask == NULL) ? RenderModeOpaque : RenderModeTransparent;

        const uint32_t scheduleFlags = renderMode == RenderModeTransparent ? gtf::scdr::ScheduleInfo::Transparent : 0;

        const gtf::scdr::ScheduleInfo info = gtf::scdr::CScheduleInfo(pipelineStateObject[renderMode], object->framebufferId, object->drawId, scheduleFlags);

        DrawTriangleCommand* const drawCmd = DrawTriangleCommand::create(commandBuffer_);

        if (drawCmd)
        {
            TriangleDrawContext& drawContext = drawCmd->drawContext;

            drawContext.pipelineStateObject = withAlphaMask ? pipelineStateObjectWithAlphaMask[renderMode] : pipelineStateObject[renderMode];
            drawContext.shaderConstants     = withAlphaMask ? &shaderConstantsWithAlphaMask : &shaderConstants;
            drawContext.uniforms            = &object->triangleParams;
            drawContext.uniformsAlphaMask   = &object->alphaMaskParams;
            drawContext.vertexBuffer        = withAlphaMask ? vertexBufferWithAlphaMask : vertexBuffer;
            drawContext.framebuffer         = object->framebuffer;
            drawContext.viewport            = &object->viewport;
            drawContext.scissor             = &object->scissor;
            drawContext.sampler             = object->alphaMask ? object->alphaMask->sampler : NULL;
            drawContext.texture             = object->alphaMask ? object->alphaMask->texture : NULL;

            commandBuffer_.enqueueRender(info, drawCmd);
        }
    }
    return true;
}

bool TriangleRenderer::end(gtf::scdr::DataStore&, gtf::scdr::CommandBuffer&, gtf::scdr::Allocator&)
{
    objects = NULL;
    return true;
}
