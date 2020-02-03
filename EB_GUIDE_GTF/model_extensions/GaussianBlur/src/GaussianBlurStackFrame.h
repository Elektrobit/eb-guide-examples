////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Elektrobit Automotive GmbH
// Alle Rechte vorbehalten. All Rights Reserved.
//
// Information contained herein is subject to change without notice.
// Elektrobit retains ownership and all other rights in the software and each
// component thereof.
// Any reproduction of the software or components thereof without the prior
// written permission of Elektrobit is prohibited.
///////////////////////////////////////////////////////////////////////////////

#ifndef GAUSSIAN_BLUR_STACK_FRAME_H
#define GAUSSIAN_BLUR_STACK_FRAME_H

#include <gtf/scdr/RenderObjectStackFrame.h>

class GaussianBlurStackFrame : public gtf::scdr::RenderObjectStackFrame2D
{
public:
    GaussianBlurStackFrame();

    void load(gtf::scdr::RenderObjectStackFrame2DValues& valuesOut_) const final override;

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

#endif // GAUSSIAN_BLUR_STACK_FRAME_H
