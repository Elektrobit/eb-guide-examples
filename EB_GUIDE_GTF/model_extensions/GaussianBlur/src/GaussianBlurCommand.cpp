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

#include "GaussianBlurCommand.h"
#include "GaussianBlurRenderObject.h"
#include <math.h>

static const uint32_t VertexStride = sizeof(GaussianBlurVertex);
static const uint32_t VertexOffset = 0;

GaussianBlurCommand::GaussianBlurCommand(GaussianBlurGraphicObjects* graphicObjects_, GaussianBlurRenderObject* renderObject_)
    : graphicObjects(graphicObjects_)
    , renderObject(renderObject_)
{
}

auto GaussianBlurCommand::blur(gtf::scdr::GPUExecutionContext& executionContext_) -> gtf::scdr::Framebuffer*
{
    static const gtf::utils::math::float2 horizontal(1.f, 0.f);
    static const gtf::utils::math::float2 vertical(0.f, 1.f);

    GTF_OSAL_ASSERT(renderObject);
    GTF_OSAL_ASSERT(renderObject->fbChildren);

    if (!renderObject->effectUniforms.enabled)
    {
        return renderObject->fbChildren;
    }

    auto colorTexture = renderObject->fbChildren->getColorAttachment();
    GTF_OSAL_ASSERT(colorTexture);

    auto        texture0          = graphicObjects->blurShaderConstants.texture0;
    auto        offsets           = graphicObjects->blurShaderConstants.offsets;
    auto        weights           = graphicObjects->blurShaderConstants.weights;
    auto        direction         = graphicObjects->blurShaderConstants.direction;
    auto        weightCount       = graphicObjects->blurShaderConstants.weightCount;
    const auto& uniformValues     = renderObject->effectUniforms;
    const auto& textureDescriptor = colorTexture->getDescriptor();

    gtf::scdr::Viewport vp;
    vp.depthFar  = 1.f;
    vp.depthNear = 0.f;
    vp.width     = textureDescriptor.width;
    vp.height    = textureDescriptor.height;
    vp.x         = 0;
    vp.y         = 0;

    gtf::scdr::Scissor scissor;
    scissor.bottom = static_cast<int16_t>(vp.height);
    scissor.right  = static_cast<int16_t>(vp.width);
    scissor.top    = 0;
    scissor.left   = 0;

    auto fbHorizontal = renderObject->fbBlur[static_cast<uint32_t>(GaussianBlurRenderObject::Pass::Horizontal)];
    auto fbVertical   = renderObject->fbBlur[static_cast<uint32_t>(GaussianBlurRenderObject::Pass::Vertical)];

    GTF_OSAL_ASSERT(fbHorizontal);
    GTF_OSAL_ASSERT(fbVertical);

    // ------ Horizontal Blur Pass ------
    gtf::scdr::DrawStateDescriptor drawStateDesc = {
        fbHorizontal,
        graphicObjects->psoBlur,
        vp,
        scissor,
        0
    };
    executionContext_.setDrawState(drawStateDesc);

    executionContext_.setTexture(*texture0, 0, colorTexture, graphicObjects->sampler0);
    executionContext_.setShaderConstant(*offsets, uniformValues.offsetx, 1, ARRAY_SIZE(uniformValues.offsetx));
    executionContext_.setShaderConstant(*weights, uniformValues.weights, 1, ARRAY_SIZE(uniformValues.weights));
    executionContext_.setShaderConstant(*direction, &horizontal.x, 2, 1);
    executionContext_.setShaderConstant(*weightCount, reinterpret_cast<const int*>(&uniformValues.weightCount), 1, 1);
    executionContext_.setVertexBuffers(&(graphicObjects->vertices), &VertexStride, &VertexOffset, 1);

    executionContext_.draw(4, 0);

    // ------ Vertical Blur Pass ------
    colorTexture = fbHorizontal->getColorAttachment();
    GTF_OSAL_ASSERT(colorTexture);

    drawStateDesc.framebuffer = fbVertical;
    executionContext_.setDrawState(drawStateDesc);

    executionContext_.setShaderConstant(*direction, &vertical.x, 2, 1);
    executionContext_.setShaderConstant(*offsets, uniformValues.offsety, 1, ARRAY_SIZE(uniformValues.offsety));
    executionContext_.setTexture(*texture0, 0, colorTexture, graphicObjects->sampler0);

    executionContext_.draw(4, 0);

    return drawStateDesc.framebuffer;
}

uint32_t GaussianBlurCommand::execute(gtf::scdr::GPUExecutionContext& executionContext_, gtf::scdr::Allocator&)
{
    auto fbInput = blur(executionContext_);
    GTF_OSAL_ASSERT(fbInput);

    const auto& shaderConstants = [this]() {
        if (renderObject->alphaMask)
        {
            return graphicObjects->shaderConstantsWithAlphaMask;
        }
        else
        {
            return graphicObjects->shaderConstants;
        }
    }();

    const auto blitPso = [this]() {
        if (renderObject->alphaMask)
        {
            return graphicObjects->psoAlphaBlit;
        }
        else
        {
            return graphicObjects->psoBlit;
        }
    }();

    const auto colorTexture = fbInput->getColorAttachment();
    GTF_OSAL_ASSERT(colorTexture);

    gtf::scdr::DrawStateDescriptor drawStateDesc;
    drawStateDesc.framebuffer = renderObject->framebuffer;
    drawStateDesc.pso         = blitPso;
    drawStateDesc.scissor     = renderObject->scissor;
    drawStateDesc.viewport    = renderObject->viewport;
    drawStateDesc.stencilRef  = 0;

    executionContext_.setDrawState(drawStateDesc);
    executionContext_.setVertexBuffers(&(graphicObjects->vertices), &VertexStride, &VertexOffset, 1);

    executionContext_.setShaderConstant(*shaderConstants.projMat, renderObject->effectUniforms.projMat.values, 16);
    executionContext_.setShaderConstant(*shaderConstants.modelMat, renderObject->effectUniforms.modelMat.values, 16);
    executionContext_.setTexture(*shaderConstants.texture0, 0, colorTexture, graphicObjects->sampler0);

    if (shaderConstants.texture1 != NULL)
    {
        executionContext_.setTexture(*shaderConstants.texture1, 1, renderObject->alphaMask->texture, renderObject->alphaMask->sampler);
        executionContext_.setShaderConstant(*shaderConstants.object2Alpha, renderObject->alphaMaskUniforms.object2Alpha.values, 16);
    }

    executionContext_.draw(4, 0);

    return gtf::scdr::CommandErrorCode::Success;
}
