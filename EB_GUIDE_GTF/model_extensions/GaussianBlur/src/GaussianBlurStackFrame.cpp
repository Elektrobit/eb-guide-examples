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

#include "GaussianBlurStackFrame.h"

GaussianBlurStackFrame::GaussianBlurStackFrame()
    : framebuffer(NULL)
    , framebufferId(0)
    , depth(0)
    , widget2AlphaMask(gtf::utils::math::float4x4::Identity())
{
    depthPtr = &depth;
}

auto GaussianBlurStackFrame::load(gtf::scdr::RenderObjectStackFrame2DValues& valuesOut_) const -> void
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
    valuesOut_.alphaMask          = nullptr;
    valuesOut_.widget2AlphaMask   = &widget2AlphaMask;
}
