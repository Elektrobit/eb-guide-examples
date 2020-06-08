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

#ifndef MULTISAMPLE_STACK_FRAME_H
#define MULTISAMPLE_STACK_FRAME_H

#include <gtf/scdr/RenderObjectStackFrame.h>
#include <gtf/scdr/RenderObjectUtils.h>

class MultisampleStackFrame : public gtf::scdr::RenderObjectStackFrame2D
{
public:
    MultisampleStackFrame();

    void load(gtf::scdr::RenderObjectStackFrame2DValues& valuesOut_) const final override;

    void store(const gtf::scdr::WidgetTransformations&   transforms_,
        const gtf::scdr::RenderObjectStackFrame2DValues& parentValues_,
        const gtf::scdr::Color&                          coloration_,
        const gtf::scdr::Scissor&                        scissor_);

private:
    const gtf::scdr::Viewport* viewport;
    gtf::scdr::Framebuffer*    framebuffer;
    uint32_t                   framebufferId;

    const gtf::utils::math::float4x4* viewProjMat;
    const gtf::utils::math::float4x4* framebuffer2Screen;
    gtf::utils::math::float4x4        widget2Framebuffer;
    gtf::utils::math::float4x4        widget2AlphaMask;
    const gtf::scdr::AlphaMask*       alphaMask;

    gtf::scdr::Scissor scissor;
    gtf::scdr::Color   coloration;
    uint32_t*          depth;
};

#endif // MULTISAMPLE_STACK_FRAME_H
