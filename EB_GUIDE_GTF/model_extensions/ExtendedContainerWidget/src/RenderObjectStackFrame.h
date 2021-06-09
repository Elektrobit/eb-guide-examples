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

#ifndef GTF_EXTENDED_CONTAINER_RENDER_OBJECT_STACK_FRAME_H_INCLUDED
#define GTF_EXTENDED_CONTAINER_RENDER_OBJECT_STACK_FRAME_H_INCLUDED

#include <gtf/scdr/RenderObjectStackFrame.h>

namespace extendedcontainerwidget
{
    class RenderObjectStackFrame : public gtf::scdr::RenderObjectStackFrame2D
    {
    public:

        void load(gtf::scdr::RenderObjectStackFrame2DValues& valuesOut_) const
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

        gtf::scdr::Color               coloration;
        gtf::scdr::Scissor             scissor;
        gtf::utils::math::float4x4            widget2Framebuffer;
        gtf::utils::math::float4x4            widget2AlphaMask;
        const gtf::utils::math::float4x4*     framebuffer2Screen;
        const gtf::utils::math::float4x4*     viewProjMat;
        const gtf::scdr::AlphaMask*    alphaMask;
        gtf::scdr::Framebuffer*        framebuffer;
        const gtf::scdr::Viewport*     viewport;
        uint32_t                       framebufferId;
        uint32_t*                      depth;
    };
}
#endif // GTF_EXTENDED_CONTAINER_RENDER_OBJECT_STACK_FRAME_H_INCLUDED
