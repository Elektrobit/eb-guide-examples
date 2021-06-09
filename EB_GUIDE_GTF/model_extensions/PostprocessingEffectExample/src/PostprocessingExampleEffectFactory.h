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

#ifndef POSTPROCESSINGEXAMPLEEFFECTFACTORY_H
#define POSTPROCESSINGEXAMPLEEFFECTFACTORY_H

#include <gtf/scdr/PostprocessingEffectFactory.h>
#include <gtf/utils/math/vector4.h>

static const char FEATURE_NAME[] = "ChromaticAberration";

struct EffectWidgetFeatureProperties
{
    enum Enum
    {
        Enabled = 0,
        Intensity,
        Count
    };

    gtf::scene::TypeResolver::FeaturePropertyReference fpr[Count];
};

class PostprocessingExampleEffectFactory : public gtf::scdr::PostprocessingEffectFactory
{
public:
    PostprocessingExampleEffectFactory(const gtf::scene::TypeResolverHandle& resolver_);

    ~PostprocessingExampleEffectFactory() = default;

    virtual bool createCustomResources(gtf::scdr::GPUContext& gpuContext_, gtf::scdr::CommandBuffer& commandBuffer_, gtf::scdr::Allocator& frameAllocator_) override;

    virtual void addShaders(gtf::scdr::PostprocessingShaderFactory& shaderFactory_, gtf::scdr::PostprocessingShaderArray& shaderArray_, const gtf::scdr::GraphicsApi& api_) override;

    virtual void createEffectPasses(gtf::scdr::EffectPassArray& effectPasses_, gtf::properties::Container* widget_, const gtf::scdr::PostprocessingEffectParams& effectParams_, gtf::scdr::Allocator& allocator_) const override;

    virtual void destroyEffectPasses(const gtf::scdr::EffectPassArray& effectPasses_, gtf::scdr::Allocator& allocator_) const override;

    virtual void removeShaders(gtf::scdr::PostprocessingShaderFactory& shaderFactory_, gtf::scdr::PostprocessingShaderArray& shaderArray_) override;

    virtual void destroyCustomResources(gtf::scdr::GPUContext& gpuContext_, gtf::scdr::CommandBuffer& commandBuffer_, gtf::scdr::Allocator& frameAllocator_) override;

    virtual gtf::scdr::PostprocessingEffectType getEffectType() const override;

    virtual uint32_t getEffectIndex() const override;

    bool isEnabled(gtf::properties::Container* widget_) const override;

    void addCustomFramebuffers(gtf::scdr::PostprocessingFramebufferDescriptorArray& framebufferDescriptors_, const gtf::scdr::FramebufferDescriptor&) const override;

    virtual uint8_t getPipelineConfiguration() const override;
    
   virtual bool isGraphicsAPISupported(const gtf::scdr::GraphicsApi& api_) const override;

  private:
    gtf::properties::Features::Feature widgetFeature;
    EffectWidgetFeatureProperties      effectWidgetProperties;

    struct
    {
        gtf::scdr::SamplerState* colorTextureSampler;

        gtf::scdr::ShaderConstant* intensity;
    } shaderConstantsPass1;
};

#endif // POSTPROCESSINGEXAMPLEEFFECTFACTORY_H
