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

#include <gtf/scdr/gpu/DrawStateDescriptor.h>
#include <gtf/scdr/gpu/GPUExecutionContext.h>
#include <gtf/scdr/gpu/FramebufferClearFlags.h>
#include <gtf/tracing/Logging.h>

#include "MultisampleCommand.h"

#define GTF_RENDERER_PLUGIN_Multisample

MultisampleCommand::MultisampleCommand(MultisampleGraphicObjects* graphicObjects_, MultisampleRenderObject* object_)
    : Command()
{
    memset(&drawContext, 0, sizeof(drawContext));

    const bool       withAlphaMask = object_->alphaMask != NULL;
    const uint32_t   width = object_->propertyValues.width;
    const uint32_t   height = object_->propertyValues.height;
    MultisampleShaderConstants& shaderConstants = graphicObjects_->shaderConstants;

    drawContext.uniforms = &object_->uniforms;
  
    drawContext.offscreenPass.fbRender     = object_->fbRender;
    drawContext.offscreenPass.fbResolve    = object_->fbResolve;
    drawContext.offscreenPass.vertexBuffer = graphicObjects_->vertexBuffer;
    drawContext.offscreenPass.pso          = graphicObjects_->psos.offscreenPSO;
    drawContext.offscreenPass.viewport     = gtf::scdr::CViewport(0, 0, width, height);;
    drawContext.offscreenPass.scissor      = gtf::scdr::CScissor((int16_t)0, (int16_t)width, (int16_t)0, (int16_t)height);
    drawContext.offscreenPass.projMat      = shaderConstants.offscreenPass.projMat;
    drawContext.offscreenPass.modelMat     = shaderConstants.offscreenPass.modelMat;
    drawContext.offscreenPass.color        = shaderConstants.offscreenPass.color;

    drawContext.defaultPass.framebuffer  = object_->framebuffer;
    drawContext.defaultPass.vertexBuffer = graphicObjects_->compositingQuadVertexBuffer;
    drawContext.defaultPass.pso          = withAlphaMask ? graphicObjects_->psos.defaultPSOWithAlphaMask : graphicObjects_->psos.defaultPSO;
    drawContext.defaultPass.viewport     = &object_->viewport;
    drawContext.defaultPass.scissor      = &object_->scissor;
    drawContext.defaultPass.sampler      = graphicObjects_->sampler;
    drawContext.defaultPass.projMat      = withAlphaMask ? shaderConstants.defaultPassWithAlphaMask.projMat :
                                                           shaderConstants.defaultPass.projMat;
    drawContext.defaultPass.modelMat     = withAlphaMask ? shaderConstants.defaultPassWithAlphaMask.modelMat :
                                                           shaderConstants.defaultPass.modelMat;
    drawContext.defaultPass.texture      = withAlphaMask ? shaderConstants.defaultPassWithAlphaMask.texture :
                                                      shaderConstants.defaultPass.texture;

    drawContext.alphaMask.sampler          = withAlphaMask ? object_->alphaMask->sampler : nullptr;
    drawContext.alphaMask.texture          = withAlphaMask ? object_->alphaMask->texture : nullptr;
    drawContext.alphaMask.object2Alpha     = shaderConstants.defaultPassWithAlphaMask.object2Alpha;
    drawContext.alphaMask.textureAlphaMask = shaderConstants.defaultPassWithAlphaMask.textureAlphaMask;
}

MultisampleCommand* MultisampleCommand::create(gtf::scdr::CommandBuffer& commandBuffer_,
    MultisampleGraphicObjects* graphicObjects_, MultisampleRenderObject* renderObject_)
{
    gtf::scdr::Allocator& commandAllocator = commandBuffer_.getCommandAllocator();

    return ALLOC_OBJECT(commandAllocator, MultisampleCommand, graphicObjects_, renderObject_);
}

uint32_t MultisampleCommand::execute(gtf::scdr::GPUExecutionContext& executionContext_, gtf::scdr::Allocator&)
{
    // Offscreen framebuffer pass
    {
        gtf::scdr::DrawStateDescriptor drawStateDesc;
        memset(&drawStateDesc, 0, sizeof(drawStateDesc));

        const gtf::scdr::Framebuffer* const fbRender  = drawContext.offscreenPass.fbRender;
        const gtf::scdr::Framebuffer* const fbResolve = drawContext.offscreenPass.fbResolve;

        if (!fbRender->isFramebufferValid() || !fbResolve->isFramebufferValid())
        {
            GTF_WARNING(GTF_RENDERER_PLUGIN_Multisample, "Failed to create multisampled framebuffers. Can not execute multisampled example rendering commands.");
            return gtf::scdr::CommandErrorCode::CriticalFailure;
        }

        drawStateDesc.framebuffer = drawContext.offscreenPass.fbRender;
        drawStateDesc.pso = drawContext.offscreenPass.pso;
        drawStateDesc.viewport = drawContext.offscreenPass.viewport;
        drawStateDesc.scissor = drawContext.offscreenPass.scissor;

        executionContext_.setDrawState(drawStateDesc);

        executionContext_.setShaderConstant(*(drawContext.offscreenPass.projMat), drawContext.uniforms->offscreenPass.projMat.values, 16);
        executionContext_.setShaderConstant(*(drawContext.offscreenPass.modelMat), drawContext.uniforms->offscreenPass.modelMat.values, 16);
        executionContext_.setShaderConstant(*(drawContext.offscreenPass.color), &(drawContext.uniforms->offscreenPass.fillColor.r), 4);

        const uint32_t offset = 0;
        gtf::scdr::VertexBuffer* const vb = drawContext.offscreenPass.vertexBuffer;
        const uint32_t stride = sizeof(MultisampleVertex);
        executionContext_.setVertexBuffers(&vb, &stride, &offset, 1);

        static const uint32_t flags = gtf::scdr::FramebufferClearFlags::Color
            | gtf::scdr::FramebufferClearFlags::Depth
            | gtf::scdr::FramebufferClearFlags::Stencil;
        executionContext_.clearFramebuffer(drawContext.offscreenPass.fbRender, flags, gtf::scdr::SystemColors::White, 1.0, 0);

        const uint32_t vertexCount = vb->getDescriptor().sizeInBytes / sizeof(MultisampleVertex);
        executionContext_.draw(vertexCount, 0);

        if (drawContext.offscreenPass.fbResolve != drawContext.offscreenPass.fbRender)
            executionContext_.blitFramebuffer(drawContext.offscreenPass.fbResolve, drawContext.offscreenPass.fbRender);
    }

    // Default framebuffer pass
    {
        gtf::scdr::DrawStateDescriptor drawStateDesc;
        memset(&drawStateDesc, 0, sizeof(drawStateDesc));

        drawStateDesc.framebuffer = drawContext.defaultPass.framebuffer;
        drawStateDesc.pso = drawContext.defaultPass.pso;
        drawStateDesc.viewport = *drawContext.defaultPass.viewport;
        drawStateDesc.scissor = *drawContext.defaultPass.scissor;

        executionContext_.setDrawState(drawStateDesc);

        if (drawContext.alphaMask.texture != nullptr)
        {
            executionContext_.setTexture(*(drawContext.alphaMask.textureAlphaMask), 1, drawContext.alphaMask.texture, drawContext.alphaMask.sampler);
            executionContext_.setShaderConstant(*(drawContext.alphaMask.object2Alpha), drawContext.uniforms->object2Alpha.values, 16);
        }
        executionContext_.setShaderConstant(*(drawContext.defaultPass.projMat), drawContext.uniforms->defaultPass.projMat.values, 16);
        executionContext_.setShaderConstant(*(drawContext.defaultPass.modelMat), drawContext.uniforms->defaultPass.quadMat.values, 16);
        {
            const gtf::scdr::Framebuffer* const fbRender = drawContext.offscreenPass.fbRender;
            const gtf::scdr::Framebuffer* const fbResolve = drawContext.offscreenPass.fbResolve;
            gtf::scdr::Texture2D* const texture = (fbResolve != fbRender) ? fbResolve->getColorAttachment() : fbRender->getColorAttachment();
            executionContext_.setTexture(*(drawContext.defaultPass.texture), 0, texture, drawContext.defaultPass.sampler);
        }

        const uint32_t offset = 0;
        uint32_t       stride = sizeof(MultisampleVertexWithUV);
        gtf::scdr::VertexBuffer* const vb = drawContext.defaultPass.vertexBuffer;
        executionContext_.setVertexBuffers(&vb, &stride, &offset, 1);
        executionContext_.draw(4, 0);
    }

    return gtf::scdr::CommandErrorCode::Success;
}
