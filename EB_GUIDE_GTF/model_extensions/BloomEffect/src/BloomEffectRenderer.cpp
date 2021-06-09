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
#include <gtf/scdr/gpu/Framebuffer.h>
#include <gtf/scdr/gpu/FramebufferClearFlags.h>
#include <gtf/scdr/gpu/GPUExecutionContext.h>
#include <gtf/scdr/RenderObject.h>
#include <gtf/scdr/SDK.h>
#include <gtf/scdr/Color.h>
#include <gtf/scdr/RenderObjectBase.h>

#include "BloomEffectRenderer.h"
#include "GLES3Shader.h"

///////// class BloomEffectStackFrame ////////////

class BloomEffectStackFrame : public gtf::scdr::RenderObjectStackFrame2D
{
public:
    BloomEffectStackFrame()
        : framebufferId(0)
        , framebuffer(NULL)
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

///////// class BloomEffectRenderObject ////////////

class BloomEffectRenderObject
    : public gtf::scdr::RenderObjectBase,
      public gtf::scdr::TouchObject
{
private:
    BloomEffectRenderObject(const BloomEffectRenderObject&);
    BloomEffectRenderObject& operator=(const BloomEffectRenderObject&);

public:
    BloomEffectRenderObject(const BloomEffectGraphicObjects& graphicObjects_, gtf::properties::Container& widget_, const gtf::scdr::WidgetFeaturesCommon2D& features_, const gtf::scdr::WidgetPropertiesCommon2D& properties_)
        : RenderObjectBase(widget_, features_, properties_)
        , next(NULL)
        , graphicObjects(graphicObjects_)
        , framebuffer(NULL)
        , offscreenFramebuffer(NULL)
        , luminanceFramebuffer(NULL)
        , gaussianBlurFramebuffer(NULL)
        , viewport(gtf::scdr::CViewport(0, 0, 0, 0))
        , alphaMask(NULL)
        , shader(BloomEffectShader::Luminance)
        , drawId(0)
        , fbid(0)
        , offscreenFbId(0)
        , luminanceFbId(0)
        , color(gtf::scdr::SystemColors::Null)
        , threshold(1.0f)
        , luminanceBlurPath(0.0f)
        , blurIterations(1)
    {
    }

    virtual gtf::scdr::RenderObjectStackFrame* createStackFrame(gtf::scdr::RenderObjectStackFrame& parent_, gtf::scdr::Allocator& allocator_)
    {
        GTF_UNUSED_PARAM(parent_);

        return ALLOC_OBJECT(allocator_, BloomEffectStackFrame);
    }

    virtual void destroyStackFrame(gtf::scdr::RenderObjectStackFrame* frame_, gtf::scdr::Allocator& allocator_)
    {
        FREE_OBJECT(allocator_, static_cast<BloomEffectStackFrame*>(frame_));
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
    BloomEffectRenderObject* next;

    const BloomEffectGraphicObjects& graphicObjects;

    gtf::scdr::Framebuffer*     framebuffer;
    gtf::scdr::Framebuffer*     offscreenFramebuffer;
    gtf::scdr::Framebuffer*     luminanceFramebuffer;
    gtf::scdr::Framebuffer*     gaussianBlurFramebuffer;
    gtf::scdr::Viewport         viewport;
    gtf::scdr::Viewport         offscreenViewport;
    gtf::utils::math::float4x4  object2Screen;
    gtf::utils::math::float4x4  object2Offscreen;
    gtf::utils::math::float4x4  alpha2Screen;
    const gtf::scdr::AlphaMask* alphaMask;
    BloomEffectShader::Value    shader;
    uint32_t                    drawId;
    uint32_t                    fbid;
    uint32_t                    offscreenFbId;
    uint32_t                    luminanceFbId;
    gtf::scdr::Color            color;
    float                       threshold;
    float                       luminanceBlurPath;
    int32_t                     blurIterations;
};

///////// free functions ////////////

static gtf::scdr::Framebuffer* CreateOrUpdateFramebuffer(
    const gtf::scdr::WidgetPropertiesCommon2DValues& properties_,
    bool                                             depthStencil_,
    gtf::scdr::Framebuffer*                          fbIn_,
    const gtf::scdr::Framebuffer*                    sysFb_,
    gtf::scdr::GPUContext&                           context_,
    gtf::scdr::CommandBuffer&                        commandBuffer_)
{
    const uint32_t width  = static_cast<uint32_t>(properties_.width);
    const uint32_t height = static_cast<uint32_t>(properties_.height);

    // Avoid framebuffers with size 0/0
    if (0 >= width || 0 >= height)
    {
        context_.destroyFramebuffer(fbIn_, commandBuffer_);
        return NULL;
    }

    gtf::scdr::FramebufferDescriptor fbdesc;

    if (fbIn_)
    {
        fbdesc = fbIn_->getDescriptor();

        if (fbdesc.width == width && fbdesc.height == height)
        {
            return fbIn_;
        }

        context_.destroyFramebuffer(fbIn_, commandBuffer_);

        fbdesc.width  = width;
        fbdesc.height = height;
    }
    else
    {
        fbdesc = sysFb_->getDescriptor();

        if (!depthStencil_)
        {
            fbdesc.depthStencilFormat = gtf::scdr::TextureFormat::Unknown;
        }

        fbdesc.width   = width;
        fbdesc.height  = height;
        fbdesc.flags   = gtf::scdr::FramebufferFlags::ColorBufferAsTexture;
        fbdesc.samples = 1;
    }

    return context_.createFramebuffer(fbdesc, commandBuffer_);
}

static bool ClearFrambuffer(gtf::scdr::Framebuffer* const fb_, gtf::scdr::GPUExecutionContext& executionContext_)
{
    static const uint32_t clearFlags =
        gtf::scdr::FramebufferClearFlags::Color | gtf::scdr::FramebufferClearFlags::Depth | gtf::scdr::FramebufferClearFlags::Stencil;

    if (fb_)
    {
        executionContext_.clearFramebuffer(fb_, clearFlags, gtf::scdr::SystemColors::Null, 1.0, 0);
        return true;
    }
    return false;
}

class ClearOffscreenFramebufferCommand : public gtf::scdr::Command
{
public:
    static ClearOffscreenFramebufferCommand* create(
        gtf::scdr::Framebuffer*   offscreenFramebuffer_,
        gtf::scdr::Framebuffer*   luminanceFramebuffer_,
        gtf::scdr::Framebuffer*   gaussianBlurFramebuffer_,
        gtf::scdr::CommandBuffer& commandBuffer_)
    {
        gtf::scdr::Allocator& cmdAllocator = commandBuffer_.getCommandAllocator();
        return ALLOC_OBJECT(cmdAllocator, ClearOffscreenFramebufferCommand, offscreenFramebuffer_, luminanceFramebuffer_, gaussianBlurFramebuffer_);
    }

    ClearOffscreenFramebufferCommand(
        gtf::scdr::Framebuffer* offscreenFramebuffer_,
        gtf::scdr::Framebuffer* luminanceFramebuffer_,
        gtf::scdr::Framebuffer* gaussianBlurFramebuffer_)
        : Command()
        , offscreenFramebuffer(offscreenFramebuffer_)
        , luminanceFramebuffer(luminanceFramebuffer_)
        , gaussianBlurFramebuffer(gaussianBlurFramebuffer_)
    {
    }

    virtual uint32_t execute(gtf::scdr::GPUExecutionContext& executionContext_, gtf::scdr::Allocator&)
    {
        bool success = ClearFrambuffer(offscreenFramebuffer, executionContext_);
        success &= ClearFrambuffer(luminanceFramebuffer, executionContext_);
        success &= ClearFrambuffer(gaussianBlurFramebuffer, executionContext_);

        if (success)
        {
            return gtf::scdr::CommandErrorCode::Success;
        }

        return gtf::scdr::CommandErrorCode::CriticalFailure;
    }

private:
    gtf::scdr::Framebuffer* offscreenFramebuffer;
    gtf::scdr::Framebuffer* luminanceFramebuffer;
    gtf::scdr::Framebuffer* gaussianBlurFramebuffer;
};

static void SetDrawState(
    gtf::scdr::Framebuffer* const        fbDest_,
    const BloomEffectRenderObject* const object_,
    const gtf::scdr::Viewport&           viewport_,
    const BloomEffectShader::Value       shader_,
    gtf::scdr::GPUExecutionContext&      executionContext_)
{
    const BloomEffectGraphicObjects& graphicObjects = object_->graphicObjects;

    // clang-format off
    const gtf::scdr::DrawStateDescriptor drawStateDesc =
    {
        fbDest_,
        graphicObjects.pso[shader_],
        viewport_,
        object_->scissor,
        0
    };
    // clang-format on

    executionContext_.setDrawState(drawStateDesc);
}

static bool DrawEffect(
    const BloomEffectRenderObject* const object_,
    gtf::scdr::GPUExecutionContext&      executionContext_)
{
    const BloomEffectGraphicObjects& graphicObjects = object_->graphicObjects;

    const uint32_t offset = 0;
    const uint32_t stride = 4 * sizeof(float);

    executionContext_.setVertexBuffers(&graphicObjects.vb, &stride, &offset, 1);
    executionContext_.draw(4, 0);

    return true;
}

static bool RenderLuminance(BloomEffectRenderObject* const object_, gtf::scdr::Framebuffer* const fbSrc_, gtf::scdr::Framebuffer* const fbDest_, gtf::scdr::GPUExecutionContext& executionContext_)
{
    if (fbSrc_ && fbDest_)
    {
        const BloomEffectShader::Value shader = BloomEffectShader::Luminance;

        SetDrawState(fbDest_, object_, object_->offscreenViewport, shader, executionContext_);

        executionContext_.setShaderConstant(*object_->graphicObjects.shaderConstants[shader].object2Screen, object_->object2Offscreen.values, 16);
        executionContext_.setShaderConstant(*object_->graphicObjects.shaderConstants[shader].lightColor, &object_->color.r, 4);
        executionContext_.setShaderConstant(*object_->graphicObjects.shaderConstants[shader].threshold, &object_->threshold, 1);

        gtf::scdr::Texture2D* const texture = fbSrc_->getColorAttachment();
        executionContext_.setTexture(*object_->graphicObjects.shaderConstants[shader].colorTexture, 0, texture, object_->graphicObjects.sampler[0]);

        DrawEffect(object_, executionContext_);
        return true;
    }
    return false;
}

static bool RenderGaussianBlur(BloomEffectRenderObject* const object_, gtf::scdr::Framebuffer* const fbSrc_, gtf::scdr::Framebuffer* const fbDest_, gtf::scdr::GPUExecutionContext& executionContext_, gtf::scdr::Framebuffer*& fbOut_)
{
    if (fbSrc_ && fbDest_)
    {
        const BloomEffectShader::Value shader[2] = {
            BloomEffectShader::GaussianBlurVert,
            BloomEffectShader::GaussianBlurHor
        };

        gtf::scdr::Texture2D* const pingPongBuffer[2] =
            {
                fbSrc_->getColorAttachment(),
                fbDest_->getColorAttachment()
            };

        bool         horizontal = false;
        unsigned int amount     = (object_->blurIterations < 1) ? 2 : object_->blurIterations * 2;

        gtf::scdr::Framebuffer* const pingPongFbo[2] = { fbSrc_, fbDest_ };
        const float                   size[2]        = { object_->width, object_->height };

        for (unsigned int i = 0; i < amount; ++i)
        {
            horizontal = !horizontal;

            SetDrawState(pingPongFbo[horizontal], object_, object_->offscreenViewport, shader[horizontal], executionContext_);

            if (i == 0 || i == 1)
            {
                executionContext_.setShaderConstant(*object_->graphicObjects.shaderConstants[shader[horizontal]].object2Screen, object_->object2Offscreen.values, 16);
                executionContext_.setShaderConstant(*object_->graphicObjects.shaderConstants[shader[horizontal]].size, size, 2);
            }
            executionContext_.setTexture(*object_->graphicObjects.shaderConstants[shader[horizontal]].colorTexture, 0,
                pingPongBuffer[!horizontal], object_->graphicObjects.sampler[0]);
            DrawEffect(object_, executionContext_);
        }

        fbOut_ = pingPongFbo[horizontal];
        return true;
    }
    return false;
}

static bool RenderFinalBloom(BloomEffectRenderObject* const object_, gtf::scdr::Framebuffer* const fbSrc1_, gtf::scdr::Framebuffer* const fbSrc2_, gtf::scdr::Framebuffer* const fbDest_, gtf::scdr::GPUExecutionContext& executionContext_)
{
    const BloomEffectShader::Value shader = object_->alphaMask ? BloomEffectShader::FinalBloomWithAlpha : BloomEffectShader::FinalBloom;

    if (fbSrc1_ && fbSrc2_ && fbDest_)
    {
        gtf::scdr::Texture2D* const texture1 = fbSrc1_->getColorAttachment();
        gtf::scdr::Texture2D* const texture2 = fbSrc2_->getColorAttachment();

        SetDrawState(fbDest_, object_, object_->viewport, shader, executionContext_);

        executionContext_.setShaderConstant(*object_->graphicObjects.shaderConstants[shader].object2Screen, object_->object2Screen.values, 16);
        executionContext_.setShaderConstant(*object_->graphicObjects.shaderConstants[shader].luminanceBlurPath, &object_->luminanceBlurPath, 1);

        executionContext_.setTexture(*object_->graphicObjects.shaderConstants[shader].offscreenTexture, 0,
            texture1, object_->graphicObjects.sampler[0]);
        executionContext_.setTexture(*object_->graphicObjects.shaderConstants[shader].offscreenTexture, 1,
            texture2, object_->graphicObjects.sampler[1]);

        if (object_->alphaMask)
        {
            executionContext_.setShaderConstant(*object_->graphicObjects.shaderConstants[shader].object2Alpha, object_->alpha2Screen.values, 16);

            executionContext_.setTexture(*object_->graphicObjects.shaderConstants[shader].alphaMaskTexture, 2,
                object_->alphaMask->texture, object_->alphaMask->sampler);
        }

        DrawEffect(object_, executionContext_);
        return true;
    }
    return false;
}

class DrawBloomEffectCommand : public gtf::scdr::Command
{
public:
    static DrawBloomEffectCommand* create(BloomEffectRenderObject* renderObject_, gtf::scdr::CommandBuffer& commandBuffer_)
    {
        gtf::scdr::Allocator& cmdAllocator = commandBuffer_.getCommandAllocator();
        return ALLOC_OBJECT(cmdAllocator, DrawBloomEffectCommand, renderObject_);
    }

    DrawBloomEffectCommand(BloomEffectRenderObject* renderObject_)
        : Command()
        , renderObject(renderObject_)
    {
    }

    virtual uint32_t execute(gtf::scdr::GPUExecutionContext& executionContext_, gtf::scdr::Allocator&)
    {
        gtf::scdr::Framebuffer* gaussianBlurFbOutput = NULL;

        bool success = true;

        success = success ? RenderLuminance(renderObject, renderObject->offscreenFramebuffer, renderObject->luminanceFramebuffer, executionContext_) : false;
        success = success ? RenderGaussianBlur(renderObject, renderObject->luminanceFramebuffer, renderObject->gaussianBlurFramebuffer, executionContext_, gaussianBlurFbOutput) : false;
        success = success ? RenderFinalBloom(renderObject, renderObject->offscreenFramebuffer, gaussianBlurFbOutput, renderObject->framebuffer, executionContext_) : false;

        if (success)
        {
            return gtf::scdr::CommandErrorCode::Success;
        }

        return gtf::scdr::CommandErrorCode::CriticalFailure;
    }

private:
    BloomEffectRenderObject* renderObject;
};

struct BloomEffectVertex
{
    float pos[2];
    float tex[2];
};

// clang-format off
static const BloomEffectVertex BloomEffectQuad[] =
{
    { { 0.f, 0.f }, { 0.f, 1.f } },
    { { 0.f, 1.f }, { 0.f, 0.f } },
    { { 1.f, 0.f }, { 1.f, 1.f } },
    { { 1.f, 1.f }, { 1.f, 0.f } }
};
// clang-format on

static bool CreateBloomEffectQuad(gtf::scdr::GPUContext& context_, gtf::scdr::CommandBuffer& commandBuffer_, gtf::scdr::VertexBuffer*& vbOut_)
{
    vbOut_ = context_.createVertexBuffer(
        gtf::scdr::CBufferDescriptor(gtf::scdr::BufferUsage::Immutable, sizeof(BloomEffectQuad)),
        BloomEffectQuad,
        commandBuffer_);

    return vbOut_ != NULL;
}

static bool CreateBloomEffectShaderProgram(gtf::scdr::GraphicsApi driver_, BloomEffectShader::Value shaderType_, gtf::scdr::GPUContext& context_, gtf::scdr::CommandBuffer& commandBuffer_, gtf::scdr::ShaderProgram*& progOut_)
{
    gtf::scdr::Shader* vs = NULL;
    gtf::scdr::Shader* fs = NULL;
    progOut_              = NULL;

    switch (driver_)
    {
    case gtf::scdr::GraphicsApi::OpenGLES3:
        vs = context_.createShader(gtf::scdr::Shader::Type::Vertex, VertexShaderGLES3, NULL, 0, commandBuffer_);
        fs = context_.createShader(gtf::scdr::Shader::Type::Fragment, FragmentShaderGLES3[shaderType_], NULL, 0, commandBuffer_);
        break;

    default:
        return false;
    }

    if (vs && fs)
    {
        const gtf::scdr::ShaderProgramSet progSet = { vs, fs };

        progOut_ = context_.createShaderProgram(progSet, commandBuffer_);
    }

    context_.destroyShader(vs, commandBuffer_);
    context_.destroyShader(fs, commandBuffer_);

    return progOut_ != NULL;
}

static bool CreateFinalBloomWithAlphaShaderProgram(gtf::scdr::GraphicsApi driver_, gtf::scdr::GPUContext& context_, gtf::scdr::CommandBuffer& commandBuffer_, gtf::scdr::ShaderProgram*& progOut_)
{
    gtf::scdr::Shader* vs = NULL;
    gtf::scdr::Shader* fs = NULL;
    progOut_              = NULL;

    switch (driver_)
    {
    case gtf::scdr::GraphicsApi::OpenGLES3:
        vs = context_.createShader(gtf::scdr::Shader::Type::Vertex, VertexShaderGLES3FinalBloomWithAlpha, NULL, 0, commandBuffer_);
        fs = context_.createShader(gtf::scdr::Shader::Type::Fragment, FragmentShaderGLES3FinalBloomWithAlpha, NULL, 0, commandBuffer_);
        break;

    default:
        return false;
    }

    if (vs && fs)
    {
        const gtf::scdr::ShaderProgramSet progSet = { vs, fs };

        progOut_ = context_.createShaderProgram(progSet, commandBuffer_);
    }

    context_.destroyShader(vs, commandBuffer_);
    context_.destroyShader(fs, commandBuffer_);

    return progOut_ != NULL;
}

struct PsoOptions // to configure pso creation
{
    enum Value
    {
        None            = 0,
        UseDepthStencil = 1 << 0,
        UseBlend        = 1 << 1,
        UseScissor      = 1 << 2
    };
};

static bool CreateBloomEffectPso(gtf::scdr::ShaderProgram* prog_, gtf::scdr::GPUContext& context_, gtf::scdr::CommandBuffer& commandBuffer_, gtf::scdr::PipelineStateObject*& psoOut_, uint32_t psoOptions_)
{
    // clang-format off
    const gtf::scdr::PipelineInputDescriptor pid[] =
    {
        { "POSITION", gtf::scdr::VertexFormat::X32Y32_FLOAT, 0, 0, gtf::scdr::InputClassification::VertexData, 0 },
        { "TEXCOORD", gtf::scdr::VertexFormat::X32Y32_FLOAT, 0, 8, gtf::scdr::InputClassification::VertexData, 0 }
    };
    // clang-format on

    gtf::scdr::PipelineStateObjectDescriptor psod;
    psod.inputDescriptorsCount = ARRAY_SIZE(pid);
    psod.inputDescriptors      = pid;
    psod.program               = prog_;
    psod.primitiveTopology     = gtf::scdr::PrimitiveTopology::TriangleStrip;

    if (PsoOptions::UseBlend & psoOptions_)
    {
        gtf::scdr::CreateAdditiveBlendStateDescriptor(psod.blendStateDescriptor);
    }
    else
    {
        gtf::scdr::CreateOpaqueBlendStateDescriptor(psod.blendStateDescriptor);
    }

    gtf::scdr::CreateDefaultDepthStencilStateDescriptor(psod.depthStencilStateDescriptor);
    if (PsoOptions::UseDepthStencil & psoOptions_)
    {
        psod.depthStencilStateDescriptor.depthEnable   = true;
        psod.depthStencilStateDescriptor.stencilEnable = true;
    }
    else
    {
        psod.depthStencilStateDescriptor.depthEnable   = false;
        psod.depthStencilStateDescriptor.stencilEnable = false;
    }

    gtf::scdr::CreateDefaultRasterizerStateDescriptor(psod.rasterizerStateDescriptor);

    psod.rasterizerStateDescriptor.scissorEnable = (psoOptions_ & PsoOptions::UseScissor) ? true : false;

    psoOut_ = context_.createPipelineStateObject(psod, commandBuffer_);

    return (psoOut_ != NULL);
}

static bool CreateBloomEffectSampler(gtf::scdr::GPUContext& context_, gtf::scdr::CommandBuffer& commandBuffer_, gtf::scdr::SamplerState*& samplerOut_)
{
    samplerOut_ = context_.createSamplerState(gtf::scdr::CSamplerStateDescriptor(gtf::scdr::TextureFilterMode::Point_MipNone), commandBuffer_);

    return samplerOut_ != NULL;
}

static bool CreateBloomEffectShaderConstants(
    BloomEffectShader::Value    shader_,
    gtf::scdr::GraphicsApi      driver_,
    gtf::scdr::ShaderProgram*   prog_,
    gtf::scdr::GPUContext&      context_,
    gtf::scdr::CommandBuffer&   commandBuffer_,
    bool                        withAlphaMask_,
    BloomEffectShaderConstants& shaderConstantsOut_)
{
    if (driver_ != gtf::scdr::GraphicsApi::OpenGLES3)
    {
        return false;
    }

    shaderConstantsOut_.object2Screen = context_.createShaderConstant("u_object2Screen", prog_, commandBuffer_);
    if (shaderConstantsOut_.object2Screen == NULL)
    {
        return false;
    }

    switch (shader_)
    {
    case BloomEffectShader::Luminance:
        shaderConstantsOut_.lightColor   = context_.createShaderConstant("u_lightColor", prog_, commandBuffer_);
        shaderConstantsOut_.threshold    = context_.createShaderConstant("u_threshold", prog_, commandBuffer_);
        shaderConstantsOut_.colorTexture = context_.createShaderConstant("colorTexture", prog_, commandBuffer_);
        if (shaderConstantsOut_.lightColor == NULL || shaderConstantsOut_.threshold == NULL || shaderConstantsOut_.colorTexture == NULL)
        {
            return false;
        }
        break;
    case BloomEffectShader::GaussianBlurVert:
    case BloomEffectShader::GaussianBlurHor:
        shaderConstantsOut_.size         = context_.createShaderConstant("u_size", prog_, commandBuffer_);
        shaderConstantsOut_.colorTexture = context_.createShaderConstant("colorTexture", prog_, commandBuffer_);
        if (shaderConstantsOut_.size == NULL || shaderConstantsOut_.colorTexture == NULL)
        {
            return false;
        }
        break;
    case BloomEffectShader::FinalBloom:
    case BloomEffectShader::FinalBloomWithAlpha:
        shaderConstantsOut_.bloomBlurTexture  = context_.createShaderConstant("bloomBlurTexture", prog_, commandBuffer_);
        shaderConstantsOut_.offscreenTexture  = context_.createShaderConstant("offscreenTexture", prog_, commandBuffer_);
        shaderConstantsOut_.luminanceBlurPath = context_.createShaderConstant("u_luminanceBlurPath", prog_, commandBuffer_);
        if (shaderConstantsOut_.bloomBlurTexture == NULL || shaderConstantsOut_.offscreenTexture == NULL || shaderConstantsOut_.luminanceBlurPath == NULL)
        {
            return false;
        }
        if (withAlphaMask_)
        {
            shaderConstantsOut_.object2Alpha     = context_.createShaderConstant("u_object2Alpha", prog_, commandBuffer_);
            shaderConstantsOut_.alphaMaskTexture = context_.createShaderConstant("alphaMaskTexture", prog_, commandBuffer_);
            if (shaderConstantsOut_.object2Alpha == NULL || shaderConstantsOut_.alphaMaskTexture == NULL)
            {
                return false;
            }
        }
        break;
    default:
        return false;
    }

    return true;
}

static void DeinitBloomEffectShaderConstants(
    BloomEffectShaderConstants& shaderConstants,
    gtf::scdr::GPUContext&      context_,
    gtf::scdr::CommandBuffer&   commandBuffer_)
{
    context_.destroyShaderConstant(shaderConstants.object2Screen, commandBuffer_);
    context_.destroyShaderConstant(shaderConstants.lightColor, commandBuffer_);
    context_.destroyShaderConstant(shaderConstants.threshold, commandBuffer_);
    context_.destroyShaderConstant(shaderConstants.colorTexture, commandBuffer_);
    context_.destroyShaderConstant(shaderConstants.size, commandBuffer_);
    context_.destroyShaderConstant(shaderConstants.bloomBlurTexture, commandBuffer_);
    context_.destroyShaderConstant(shaderConstants.offscreenTexture, commandBuffer_);
    context_.destroyShaderConstant(shaderConstants.luminanceBlurPath, commandBuffer_);
    context_.destroyShaderConstant(shaderConstants.object2Alpha, commandBuffer_);
    context_.destroyShaderConstant(shaderConstants.alphaMaskTexture, commandBuffer_);
    memset(&shaderConstants, 0, sizeof(shaderConstants));
}

///////// class BloomEffectRenderer ////////////

gtf::scdr::IWidgetRenderer* BloomEffectRenderer::createIntance(
    const gtf::scene::TypeResolver& resolver_,
    gtf::scdr::DataStore&           dataStore_,
    gtf::scdr::CommandBuffer&       commandBuffer_,
    gtf::scdr::Allocator&           frameAlloactor_,
    const void*)
{
    BIND_ALLOCATOR(allocator, dataStore_);

    gtf::scdr::IWidgetRenderer* renderer = ALLOC_OBJECT(allocator, BloomEffectRenderer, resolver_, dataStore_.getSystemConfig().getGraphicsApi());

    if (renderer)
    {
        if (renderer->init(resolver_, dataStore_, commandBuffer_, frameAlloactor_))
        {
            return renderer;
        }
        renderer->deinit(dataStore_, commandBuffer_, frameAlloactor_);
        FREE_OBJECT(allocator, renderer);
    }

    return NULL;
}

void BloomEffectRenderer::destroyInstance(
    gtf::scdr::IWidgetRenderer* renderer_,
    gtf::scdr::DataStore&       dataStore_,
    gtf::scdr::CommandBuffer&   commandBuffer_,
    gtf::scdr::Allocator&       frameAlloactor_)
{
    if (renderer_)
    {
        renderer_->deinit(dataStore_, commandBuffer_, frameAlloactor_);
        BIND_ALLOCATOR(allocator, dataStore_);
        FREE_OBJECT(allocator, static_cast<BloomEffectRenderer*>(renderer_));
    }
}

BloomEffectRenderer::BloomEffectRenderer(const gtf::scene::TypeResolver& resolver_, gtf::scdr::GraphicsApi driver_)
{
    gtf::scdr::ResolvePropertiesCommon2D(resolver_, BLOOMEFFECT_PLUGIN_NAME, properties.common2d);
    gtf::scdr::ResolveFeaturesCommon2D(resolver_, features.common2d);

    graphicObjects        = BloomEffectGraphicObjects();
    graphicObjects.driver = driver_;

    // clang-format off
    gtf::scene::TypeResolver::ContainerPropertyReference wpr[] =
    {
        { { BLOOMEFFECT_PLUGIN_NAME, gtf::properties::Container::Type() }, { "luminosityRGB"    , gtf::types::limits::uint16_max } },
        { { BLOOMEFFECT_PLUGIN_NAME, gtf::properties::Container::Type() }, { "threshold"        , gtf::types::limits::uint16_max } },
        { { BLOOMEFFECT_PLUGIN_NAME, gtf::properties::Container::Type() }, { "luminanceBlurPath", gtf::types::limits::uint16_max } },
        { { BLOOMEFFECT_PLUGIN_NAME, gtf::properties::Container::Type() }, { "blurIterations"   , gtf::types::limits::uint16_max } }
    };
    // clang-format on

    resolver_.resolveContainerProperties(wpr, ARRAY_SIZE(wpr));

    properties.luminosityRGB     = wpr[0].property.identifier;
    properties.threshold         = wpr[1].property.identifier;
    properties.luminanceBlurPath = wpr[2].property.identifier;
    properties.blurIterations    = wpr[3].property.identifier;
}

BloomEffectRenderer::~BloomEffectRenderer()
{
    GTF_OSAL_ASSERT(graphicObjects.sampler[0] == NULL);
    GTF_OSAL_ASSERT(graphicObjects.sampler[1] == NULL);
    GTF_OSAL_ASSERT(graphicObjects.vb == NULL);
    for (unsigned int i = 0; i < BloomEffectShader::Count; ++i)
    {
        GTF_OSAL_ASSERT(graphicObjects.pso[i] == NULL);
    }
}

bool BloomEffectRenderer::init(
    const gtf::scene::TypeResolver&,
    gtf::scdr::DataStore&     dataStore_,
    gtf::scdr::CommandBuffer& commandBuffer_,
    gtf::scdr::Allocator&)
{
    BIND_GPU_CONTEXT(gc, dataStore_);

    bool                      success = true;
    gtf::scdr::ShaderProgram* program = NULL;

    success = CreateBloomEffectQuad(gc, commandBuffer_, this->graphicObjects.vb);

    //LuminanceShader
    BloomEffectShader::Value shaderId = BloomEffectShader::Luminance;
    success                           = success ? CreateBloomEffectShaderProgram(graphicObjects.driver, shaderId, gc, commandBuffer_, program) : false;
    success                           = success ? CreateBloomEffectPso(program, gc, commandBuffer_, graphicObjects.pso[shaderId], PsoOptions::None) : false;
    success                           = success ? CreateBloomEffectShaderConstants(shaderId, graphicObjects.driver, program, gc, commandBuffer_, false, graphicObjects.shaderConstants[shaderId]) : false;

    if (program)
    {
        gc.destroyShaderProgram(program, commandBuffer_);
    }

    //GaussianBlurHor
    shaderId = BloomEffectShader::GaussianBlurHor;
    success  = success ? CreateBloomEffectShaderProgram(graphicObjects.driver, shaderId, gc, commandBuffer_, program) : false;
    success  = success ? CreateBloomEffectPso(program, gc, commandBuffer_, graphicObjects.pso[shaderId], PsoOptions::None) : false;
    success  = success ? CreateBloomEffectShaderConstants(shaderId, graphicObjects.driver, program, gc, commandBuffer_, false, graphicObjects.shaderConstants[shaderId]) : false;

    if (program)
    {
        gc.destroyShaderProgram(program, commandBuffer_);
    }

    //GaussianBlurVert
    shaderId = BloomEffectShader::GaussianBlurVert;
    success  = success ? CreateBloomEffectShaderProgram(graphicObjects.driver, shaderId, gc, commandBuffer_, program) : false;
    success  = success ? CreateBloomEffectPso(program, gc, commandBuffer_, graphicObjects.pso[shaderId], PsoOptions::None) : false;
    success  = success ? CreateBloomEffectShaderConstants(shaderId, graphicObjects.driver, program, gc, commandBuffer_, false, graphicObjects.shaderConstants[shaderId]) : false;

    if (program)
    {
        gc.destroyShaderProgram(program, commandBuffer_);
    }

    const uint32_t psoDepthBlendScissor = PsoOptions::UseDepthStencil | PsoOptions::UseBlend | PsoOptions::UseScissor;
    //FinalBloom
    shaderId = BloomEffectShader::FinalBloom;
    success  = success ? CreateBloomEffectShaderProgram(graphicObjects.driver, shaderId, gc, commandBuffer_, program) : false;
    success  = success ? CreateBloomEffectPso(program, gc, commandBuffer_, graphicObjects.pso[shaderId], psoDepthBlendScissor) : false;
    success  = success ? CreateBloomEffectShaderConstants(shaderId, graphicObjects.driver, program, gc, commandBuffer_, false, graphicObjects.shaderConstants[shaderId]) : false;

    if (program)
    {
        gc.destroyShaderProgram(program, commandBuffer_);
    }

    //FinalBloomWithAlpha
    shaderId = BloomEffectShader::FinalBloomWithAlpha;
    success  = success ? CreateFinalBloomWithAlphaShaderProgram(graphicObjects.driver, gc, commandBuffer_, program) : false;
    success  = success ? CreateBloomEffectPso(program, gc, commandBuffer_, graphicObjects.pso[shaderId], psoDepthBlendScissor) : false;
    success  = success ? CreateBloomEffectShaderConstants(shaderId, graphicObjects.driver, program, gc, commandBuffer_, true, graphicObjects.shaderConstants[shaderId]) : false;

    if (program)
    {
        gc.destroyShaderProgram(program, commandBuffer_);
    }

    if (success)
    {
        success &= CreateBloomEffectSampler(gc, commandBuffer_, graphicObjects.sampler[0]);
        success &= CreateBloomEffectSampler(gc, commandBuffer_, graphicObjects.sampler[1]);
        success &= CreateBloomEffectSampler(gc, commandBuffer_, graphicObjects.samplerAlphaMask);
    }

    return success;
}

void BloomEffectRenderer::deinit(
    gtf::scdr::DataStore&     dataStore_,
    gtf::scdr::CommandBuffer& commandBuffer_,
    gtf::scdr::Allocator&)
{
    BIND_GPU_CONTEXT(gc, dataStore_);

    gc.destroyVertexBuffer(graphicObjects.vb, commandBuffer_);
    for (unsigned int i = 0; i < BloomEffectShader::Count; ++i)
    {
        gc.destroyPipelineStateObject(graphicObjects.pso[i], commandBuffer_);
        DeinitBloomEffectShaderConstants(graphicObjects.shaderConstants[i], gc, commandBuffer_);
    }

    gc.destroySamplerState(graphicObjects.samplerAlphaMask, commandBuffer_);
    gc.destroySamplerState(graphicObjects.sampler[1], commandBuffer_);
    gc.destroySamplerState(graphicObjects.sampler[0], commandBuffer_);

    graphicObjects.vb = NULL;

    memset(&graphicObjects.sampler, 0, sizeof(graphicObjects.sampler));
    memset(&graphicObjects.pso, 0, sizeof(graphicObjects.pso));
}

gtf::scdr::RenderObject* BloomEffectRenderer::createRenderObject(
    gtf::properties::Container& widget_,
    gtf::scdr::DataStore&       dataStore_,
    gtf::scdr::CommandBuffer&,
    gtf::scdr::Allocator&)
{
    BIND_ALLOCATOR(allocator, dataStore_);

    BloomEffectRenderObject* const object = ALLOC_OBJECT(allocator, BloomEffectRenderObject, graphicObjects, widget_, features.common2d, properties.common2d);
    return object;
}

void BloomEffectRenderer::destroyRenderObject(
    gtf::scdr::RenderObject*  object_,
    gtf::scdr::DataStore&     dataStore_,
    gtf::scdr::CommandBuffer& commandBuffer_,
    gtf::scdr::Allocator&)
{
    BIND_ALLOCATOR(allocator, dataStore_);
    BIND_GPU_CONTEXT(gc, dataStore_);

    BloomEffectRenderObject* const object = static_cast<BloomEffectRenderObject*>(object_);

    if (object)
    {
        gc.destroyFramebuffer(object->offscreenFramebuffer, commandBuffer_);
        gc.destroyFramebuffer(object->luminanceFramebuffer, commandBuffer_);
        gc.destroyFramebuffer(object->gaussianBlurFramebuffer, commandBuffer_);

        FREE_OBJECT(allocator, object);
    }
}

bool BloomEffectRenderer::update(
    gtf::scdr::ResourceLoader&,
    gtf::scdr::DataStore&,
    gtf::scdr::CommandBuffer&,
    gtf::scdr::Allocator&)
{
    return true;
}

bool BloomEffectRenderer::render(
    gtf::scdr::DataStore&,
    gtf::scdr::CommandBuffer& commandBuffer_,
    gtf::scdr::Allocator&)
{
    for (BloomEffectRenderObject* object = objects; object; object = object->next)
    {
        gtf::scdr::Command* const clearCmd = ClearOffscreenFramebufferCommand::create(object->offscreenFramebuffer, object->luminanceFramebuffer, object->gaussianBlurFramebuffer, commandBuffer_);
        if (clearCmd == NULL)
        {
            return false;
        }

        const gtf::scdr::ScheduleInfo clearInfo = gtf::scdr::CScheduleInfo(object->offscreenFbId, 0, gtf::scdr::ScheduleInfo::Clear);
        commandBuffer_.enqueueRender(clearInfo, clearCmd);

        gtf::scdr::Command* const drawCmd = DrawBloomEffectCommand::create(object, commandBuffer_);
        if (drawCmd == NULL)
        {
            return false;
        }

        gtf::scdr::ScheduleInfo scheduleInfo = gtf::scdr::CScheduleInfo(
            graphicObjects.pso[BloomEffectShader::Luminance],
            object->fbid,
            object->drawId,
            gtf::scdr::CScheduleInfo::Transparent);

        commandBuffer_.enqueueRender(scheduleInfo, drawCmd);
    }

    return true;
}

bool BloomEffectRenderer::loadShaderParameters(
    gtf::properties::Container& widget_,
    BloomEffectRenderObject&    object_)
{
    bool success = true;

    success &= gtf::scdr::LoadValue(widget_, properties.luminosityRGB, object_.color);
    success &= gtf::scdr::LoadValue(widget_, properties.threshold, object_.threshold);
    success &= gtf::scdr::LoadValue(widget_, properties.luminanceBlurPath, object_.luminanceBlurPath);

    int32_t blurIterations;
    success &= gtf::scdr::LoadValue(widget_, properties.blurIterations, blurIterations);
    object_.blurIterations = MINMAX(0, blurIterations, 20);

    return success;
}

bool BloomEffectRenderer::traverse(
    gtf::properties::Container&              widget_,
    const gtf::scdr::RenderObjectStackFrame& parentStackFrame_,
    gtf::scdr::RenderObjectStackFrame&       objectStackFrame_,
    gtf::scdr::RenderObject&                 object_,
    gtf::scdr::TraverserContext&             traverserContext_)
{
    gtf::scdr::CommandBuffer& commandBuffer = traverserContext_.getCommandBuffer();
    gtf::scdr::DataStore&     dataStore     = traverserContext_.getDataStore();
    BIND_GPU_CONTEXT(gc, dataStore);

    BloomEffectStackFrame&   objectFrame = static_cast<BloomEffectStackFrame&>(objectStackFrame_);
    BloomEffectRenderObject& object      = static_cast<BloomEffectRenderObject&>(object_);

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

    if (!loadShaderParameters(widget_, object))
    {
        return false;
    }

    object.offscreenFramebuffer    = CreateOrUpdateFramebuffer(object.propertyValues, true, object.offscreenFramebuffer, stackValues.framebuffer, gc, commandBuffer);
    object.luminanceFramebuffer    = CreateOrUpdateFramebuffer(object.propertyValues, false, object.luminanceFramebuffer, stackValues.framebuffer, gc, commandBuffer);
    object.gaussianBlurFramebuffer = CreateOrUpdateFramebuffer(object.propertyValues, false, object.gaussianBlurFramebuffer, stackValues.framebuffer, gc, commandBuffer);

    if (object.offscreenFramebuffer == NULL || object.luminanceFramebuffer == NULL || object.gaussianBlurFramebuffer == NULL)
    {
        return false;
    }

    const uint32_t maxDepth = gtf::scdr::GetMaxFramebufferDepth(object.offscreenFramebuffer);
    if (maxDepth == 0)
    {
        return false;
    }

    gtf::utils::math::float4x4 offscreenViewProjMat;
    gtf::scdr::CalculateViewProjectionMatrix2D(
        object.propertyValues.width,
        object.propertyValues.height,
        maxDepth,
        graphicObjects.driver,
        offscreenViewProjMat);

    uint32_t drawDepth;
    if (!gtf::scdr::InitializeDrawDepthForRenderObjectStackFrame(*object.offscreenFramebuffer, drawDepth))
    {
        return false;
    }

    object.object2Screen = (*stackValues.viewProjMat * object.transforms.widget2Framebuffer * gtf::utils::math::float4x4::Scaling2D(object.width, object.height) * gtf::utils::math::float4x4::Translation(0.f, 0.f, static_cast<float>(drawId)))
                               .transpose();

    object.object2Offscreen = (offscreenViewProjMat * gtf::utils::math::float4x4::Scaling2D(object.width, object.height))
                                  .transpose();

    object.drawId            = drawId;
    object.fbid              = stackValues.framebufferId;
    object.framebuffer       = stackValues.framebuffer;
    object.viewport          = *stackValues.viewport;
    object.offscreenViewport = gtf::scdr::CViewport(0, 0, static_cast<uint32_t>(object.propertyValues.width), static_cast<uint32_t>(object.propertyValues.height));
    object.offscreenFbId     = traverserContext_.createFramebufferId();
    object.luminanceFbId     = traverserContext_.createFramebufferId();
    object.alphaMask         = stackValues.alphaMask;
    object.alpha2Screen      = object.transforms.widget2AlphaMask * gtf::utils::math::float4x4::Scaling2D(object.width, object.height);

    objectFrame.viewport           = gtf::scdr::CViewport(0, 0, static_cast<uint32_t>(object.propertyValues.width), static_cast<uint32_t>(object.propertyValues.height));
    objectFrame.scissor            = gtf::scdr::CScissor(0, stackValues.scissor->right - stackValues.scissor->left, 0, stackValues.scissor->bottom - stackValues.scissor->top);
    objectFrame.framebuffer2Screen = *stackValues.framebuffer2Screen * object.transforms.widget2Framebuffer;
    objectFrame.framebuffer        = object.offscreenFramebuffer;
    objectFrame.framebufferId      = object.offscreenFbId;
    objectFrame.depth              = drawDepth;
    objectFrame.viewProjMat        = offscreenViewProjMat;
    objectFrame.widget2AlphaMask   = gtf::utils::math::float4x4::Identity();
    objectFrame.coloration         = object.coloration;

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

bool BloomEffectRenderer::begin(
    gtf::scdr::DataStore&,
    gtf::scdr::CommandBuffer&,
    gtf::scdr::Allocator&)
{
    this->objects = NULL;
    return true;
}

bool BloomEffectRenderer::end(
    gtf::scdr::DataStore&,
    gtf::scdr::CommandBuffer&,
    gtf::scdr::Allocator&)
{
    return true;
}
