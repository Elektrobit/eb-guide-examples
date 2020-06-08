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

#include "MultisampleStackFrame.h"

MultisampleStackFrame::MultisampleStackFrame()
    : framebuffer(nullptr)
    , framebufferId(0)
    , widget2Framebuffer(gtf::utils::math::float4x4::Identity())
    , widget2AlphaMask(gtf::utils::math::float4x4::Identity())
    , depth(0)
{   
}

void MultisampleStackFrame::load(gtf::scdr::RenderObjectStackFrame2DValues& valuesOut_) const
{
    valuesOut_.coloration = &coloration;
    valuesOut_.framebuffer = framebuffer;
    valuesOut_.framebufferId = framebufferId;
    valuesOut_.widget2Framebuffer = &widget2Framebuffer;
    valuesOut_.framebuffer2Screen = framebuffer2Screen;
    valuesOut_.scissor = &scissor;
    valuesOut_.viewport = viewport;
    valuesOut_.viewProjMat = viewProjMat;
    valuesOut_.depth = depth;
    valuesOut_.alphaMask = alphaMask;
    valuesOut_.widget2AlphaMask = &widget2AlphaMask;
}

void MultisampleStackFrame::store(const gtf::scdr::WidgetTransformations&   transforms_,
    const gtf::scdr::RenderObjectStackFrame2DValues& parentValues_,
    const gtf::scdr::Color&                          coloration_,
    const gtf::scdr::Scissor&                        scissor_)
{
    coloration = coloration_;
    framebuffer = parentValues_.framebuffer;
    framebufferId = parentValues_.framebufferId;
    widget2Framebuffer = transforms_.widget2StackFrame;
    framebuffer2Screen = parentValues_.framebuffer2Screen;
    scissor = scissor_;
    viewport = parentValues_.viewport;
    viewProjMat = parentValues_.viewProjMat;
    depth = parentValues_.depth;
    alphaMask = parentValues_.alphaMask;
    widget2AlphaMask = transforms_.widget2AlphaMask;
}
