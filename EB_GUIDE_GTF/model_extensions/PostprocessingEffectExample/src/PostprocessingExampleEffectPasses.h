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
#ifndef POSTPROCESSINGEXAMPLEEFFECT_H
#define POSTPROCESSINGEXAMPLEEFFECT_H

#include <gtf/scdr/PostprocessingPass.h>
#include <gtf/properties/ScalarValue.h>
#include <gtf/scdr/gpu/Framebuffer.h>
#include <gtf/scdr/ShaderConstantData.h>

struct EffectPasses
{
    enum Value
    {
        Pass1 = 0,
        Count
    };
};

struct ShaderFloat
{
    gtf::scdr::ShaderConstant* uniform;
    float                      uniformValue;
};

class EffectPass1 : public gtf::scdr::PostprocessingPass
{
public:
    EffectPass1(gtf::scdr::Framebuffer* framebuffer_);

    struct
    {
        ShaderFloat intensity;
    } shaderResources;

    struct
    {
        gtf::properties::FloatValueHandle intensity;
    } widgetProperties;

    gtf::scdr::Framebuffer* framebuffer;

    // Inherited via PostprocessingPass
    virtual void updateUniformValues() override;

    virtual void setupShaderConstants(gtf::container::Array<gtf::scdr::ShaderConstantData>& shaderConstantDataOut_) const override;

    virtual uint8_t getShaderIndex() const override;

    virtual gtf::scdr::Framebuffer* getFramebuffer() const override;
};

#endif // POSTPROCESSINGEXAMPLEEFFECT_H
