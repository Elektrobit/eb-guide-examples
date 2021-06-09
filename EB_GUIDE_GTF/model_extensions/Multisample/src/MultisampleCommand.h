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

#ifndef MULTISAMPLE_COMMAND_H
#define MULTISAMPLE_COMMAND_H

#include <gtf/scdr/command/Command.h>

#include "MultisampleRenderObject.h"
#include "Multisample.h"

struct DrawContext
{
    MultisampleUniforms* uniforms;

    struct
    {
        gtf::scdr::Viewport             viewport;
        gtf::scdr::Scissor              scissor;
        gtf::scdr::Framebuffer*         fbRender;
        gtf::scdr::Framebuffer*         fbResolve;
        gtf::scdr::VertexBuffer*        vertexBuffer;
        gtf::scdr::PipelineStateObject* pso;
        gtf::scdr::ShaderConstant*      projMat;
        gtf::scdr::ShaderConstant*      modelMat;
        gtf::scdr::ShaderConstant*      color;
    } offscreenPass;

    struct
    {
        const gtf::scdr::Viewport*      viewport;
        const gtf::scdr::Scissor*       scissor;
        gtf::scdr::Framebuffer*         framebuffer;
        gtf::scdr::VertexBuffer*        vertexBuffer;
        gtf::scdr::PipelineStateObject* pso;
        gtf::scdr::SamplerState*        sampler;
        gtf::scdr::ShaderConstant*      projMat;
        gtf::scdr::ShaderConstant*      modelMat;
        gtf::scdr::ShaderConstant*      texture;
    } defaultPass;

    struct
    {
        gtf::scdr::Texture2D*      texture;
        gtf::scdr::SamplerState*   sampler;
        gtf::scdr::ShaderConstant* object2Alpha;
        gtf::scdr::ShaderConstant* textureAlphaMask;
    } alphaMask;
};

class MultisampleCommand : public gtf::scdr::Command
{
public:
    MultisampleCommand(MultisampleGraphicObjects* graphicObjects_, MultisampleRenderObject* renderObject_);

    virtual ~MultisampleCommand() override {}

    static MultisampleCommand* create(gtf::scdr::CommandBuffer& commandBuffer_, MultisampleGraphicObjects* graphicObjects_, MultisampleRenderObject* object_);

    uint32_t execute(gtf::scdr::GPUExecutionContext& executionContext_, gtf::scdr::Allocator&) override;

public:
    DrawContext drawContext;
};

#endif // MULTISAMPLE_COMMAND_H
