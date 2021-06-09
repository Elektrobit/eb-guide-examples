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

#include "PostprocessingExampleEffectPasses.h"

#include <gtf/scdr/gpu/GPUExecutionContext.h>

EffectPass1::EffectPass1()
{
    memset(&shaderResources, 0, sizeof(shaderResources));
}

uint8_t EffectPass1::getShaderIndex() const
{
    return EffectPasses::Pass1;
}

int8_t EffectPass1::getFramebufferIndex() const
{
    return gtf::scdr::DEFAULT_FRAMEBUFFER_INDEX;
}

void EffectPass1::updateUniformValues()
{
    shaderResources.intensity.uniformValue = widgetProperties.intensity->get();
}

void EffectPass1::setupShaderConstants(gtf::container::Array<gtf::scdr::ShaderConstantData>& shaderConstantDataOut_) const
{
    shaderConstantDataOut_.append(gtf::scdr::CShaderConstantData(shaderResources.intensity.uniform, &shaderResources.intensity.uniformValue, 1));
}
