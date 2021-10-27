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

#ifndef GAUSSIAN_BLUR_COMMAND_H
#define GAUSSIAN_BLUR_COMMAND_H

#include <gtf/scdr/command/Command.h>
#include <gtf/scdr/gpu/GPUExecutionContext.h>

#include "GaussianBlur.h"

class GaussianBlurCommand : public gtf::scdr::Command
{
public:
    GaussianBlurCommand(GaussianBlurGraphicObjects* graphicObjects_, GaussianBlurRenderObject* renderObject_);

    uint32_t execute(gtf::scdr::GPUExecutionContext& executionContext_, gtf::scdr::Allocator&);

private:
    gtf::scdr::Framebuffer* blur(gtf::scdr::GPUExecutionContext& executionContext_);

protected:
    GaussianBlurGraphicObjects* graphicObjects;
    GaussianBlurRenderObject*   renderObject;
};

#endif // GAUSSIAN_BLUR_COMMAND_H
