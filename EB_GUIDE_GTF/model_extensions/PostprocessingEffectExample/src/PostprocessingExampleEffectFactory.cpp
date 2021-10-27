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

#include "PostprocessingExampleEffectFactory.h"
#include "GLES20Shader.h"
#include "GLES3Shader.h"
#include "PostprocessingExampleEffectPasses.h"

#include <gtf/scdr/allocator/Allocator.h>
#include <gtf/utils/math/vector2.h>
#include <gtf/scdr/RenderObjectUtils.h>
#include <gtf/tracing/Logging.h>

#define POSTPROCESSINGEXAMPLE_Warning

PostprocessingExampleEffectFactory::PostprocessingExampleEffectFactory(
    const gtf::scene::TypeResolverHandle& resolver_)
{
    widgetFeature = resolver_->resolveFeatureType(FEATURE_NAME);

    // clang-format off
	gtf::scene::TypeResolver::FeaturePropertyReference fpr[] =
	{
		{ { FEATURE_NAME, gtf::properties::Features::Feature() }, { "enabled", gtf::types::limits::uint16_max } },   // 0        
		{ { FEATURE_NAME, gtf::properties::Features::Feature() }, { "intensity", gtf::types::limits::uint16_max } },   // 1        
	};
    // clang-format on

    resolver_->resolveFeatureProperties(fpr, ARRAY_SIZE(fpr));

    effectWidgetProperties.fpr[EffectWidgetFeatureProperties::Enabled]   = fpr[0];
    effectWidgetProperties.fpr[EffectWidgetFeatureProperties::Intensity] = fpr[1];
}

bool PostprocessingExampleEffectFactory::createCustomResources(gtf::scdr::GPUContext&, gtf::scdr::CommandBuffer&, gtf::scdr::Allocator&)
{
    return true;
}

void PostprocessingExampleEffectFactory::addShaders(gtf::scdr::PostprocessingShaderFactory& shaderFactory_, gtf::scdr::PostprocessingShaderArray& shaderArray_, const gtf::scdr::GraphicsApi& api_)
{
    const char* shaderCode = nullptr;

    switch (api_)
    {
    case gtf::scdr::GraphicsApi::OpenGLES20:
        shaderCode = f_chromaticAberrationShader_GLES20;
        break;
    case gtf::scdr::GraphicsApi::OpenGLES3:
        shaderCode = f_chromaticAberrationShader_GLES3;
        break;
    default:
        GTF_WARNING(POSTPROCESSINGEXAMPLE_Warning, getEffectIndex() << ": Shader code does not exist for the current graphics API.");
        break;
    }

    gtf::scdr::BlendStateDescriptor bsDesc;
    CreateDefaultBlendStateDescriptor(bsDesc);

    gtf::scdr::PostprocessingShader* shader = shaderFactory_.create(shaderCode, EffectPasses::Pass1, bsDesc);

    shader->addShaderConstant(shaderConstantsPass1.intensity, "u_intensity");

    const gtf::scdr::SamplerStateDescriptor colorTextureSamplerDesc = gtf::scdr::CSamplerStateDescriptor(gtf::scdr::TextureFilterMode::Linear_MipNone);

    shader->addColorTextureInput(colorTextureSamplerDesc, "u_texture0");

    shaderArray_.append(shader);
}

void PostprocessingExampleEffectFactory::createEffectPasses(gtf::scdr::EffectPassArray& effectPasses_, gtf::properties::Container* widget_, const gtf::scdr::PostprocessingEffectParams& effectParams_, const gtf::scdr::SystemCapabilities&, gtf::scdr::FramebufferPool& framebufferPool_, gtf::scdr::CommandBuffer& commandBuffer_, gtf::scdr::Allocator& allocator_) const
{
    effectPasses_.resize(EffectPasses::Count);

    // Acquire a free framebuffer from the FramebufferPool to set up the render output for the EffectPass.
    // Note: The last output framebuffer of an effect is auto-released by the pipeline, and must not released manually.
    gtf::scdr::Framebuffer* outputFb = framebufferPool_.acquire(createDefaultFramebufferDescriptor(effectParams_.inputFramebufferDesc), commandBuffer_);

    if (outputFb)
    {
        // Create Pass
        EffectPass1* pass = ALLOC_OBJECT(allocator_, EffectPass1, outputFb);

        if (pass)
        {
            effectPasses_[EffectPasses::Pass1] = pass;

            pass->shaderResources.intensity.uniform = shaderConstantsPass1.intensity;

            gtf::properties::Properties& widgetProperties = widget_->getProperties();
            pass->widgetProperties.intensity              = widgetProperties.get<gtf::properties::FloatValue>(effectWidgetProperties.fpr[EffectWidgetFeatureProperties::Intensity].featureType.identifier, effectWidgetProperties.fpr[EffectWidgetFeatureProperties::Intensity].property.identifier);
        }
    }
}

void PostprocessingExampleEffectFactory::destroyEffectPasses(const gtf::scdr::EffectPassArray& effectPasses_, gtf::scdr::Allocator& allocator_) const
{
    for (const auto& pass : effectPasses_)
    {
        FREE_OBJECT(allocator_, pass);
    }
}

void PostprocessingExampleEffectFactory::destroyCustomResources(gtf::scdr::GPUContext&, gtf::scdr::CommandBuffer&, gtf::scdr::Allocator&)
{
}

bool PostprocessingExampleEffectFactory::isEnabled(gtf::properties::Container* widget_) const
{
    const bool isApplied = widget_->getFeatures().has(widgetFeature);

    if (isApplied)
    {
        gtf::properties::Properties&     widgetProperties = widget_->getProperties();
        gtf::properties::BoolValueHandle enabled          = widgetProperties.get<gtf::properties::BoolValue>(effectWidgetProperties.fpr[EffectWidgetFeatureProperties::Enabled].featureType.identifier, effectWidgetProperties.fpr[EffectWidgetFeatureProperties::Enabled].property.identifier);
        return (enabled.valid() && enabled->get());
    }

    return false;
}

uint8_t PostprocessingExampleEffectFactory::getPipelineConfiguration() const
{
    return gtf::scdr::PostprocessingPipelineConfigurationFlags::None;
}

bool PostprocessingExampleEffectFactory::isGraphicsAPISupported(const gtf::scdr::GraphicsApi& api_) const
{
    switch (api_)
    {
    case gtf::scdr::GraphicsApi::OpenGLES20:
    case gtf::scdr::GraphicsApi::OpenGLES3:
        return true;
    default:
        return false;
    }
}

void PostprocessingExampleEffectFactory::removeShaders(gtf::scdr::PostprocessingShaderFactory& shaderFactory_, gtf::scdr::PostprocessingShaderArray& shaderArray_)
{
    for (const auto& shader : shaderArray_)
    {
        shaderFactory_.destroy(shader);
    }
}

gtf::scdr::PostprocessingEffectType PostprocessingExampleEffectFactory::getEffectType() const
{
    return gtf::scdr::PostprocessingEffectType::SceneEffect;
}

uint32_t PostprocessingExampleEffectFactory::getEffectIndex() const
{
    return gtf::scdr::PostprocessingPipelineBuiltin::FXAA + 1;
}
