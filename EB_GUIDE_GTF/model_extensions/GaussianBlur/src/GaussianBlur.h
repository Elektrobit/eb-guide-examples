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

#ifndef GAUSSIAN_BLUR_H
#define GAUSSIAN_BLUR_H

#include <gtf/scdr/RenderObjectUtils.h>
#include <gtf/scdr/gpu/Framebuffer.h>
#include <gtf/scdr/gpu/SamplerState.h>
#include <gtf/scdr/gpu/ShaderConstant.h>
#include <gtf/scdr/widgets/IWidgetRenderer.h>
#include <gtf/utils/math/matrix4x4.h>

class GaussianBlurRenderObject;

struct GaussianBlurVertex
{
    float posX;
    float posY;
    float texU;
    float texV;
};

struct GaussianBlurProperties
{
    gtf::scdr::WidgetPropertiesCommon2D common2d;
    uint16_t                            enabled;
    uint16_t                            sigma;
};

struct GaussianBlurFeatures
{
    gtf::scdr::WidgetFeaturesCommon2D common2d;
};

struct BlurPassShaderConstants
{
    gtf::scdr::ShaderConstant* texture0;
    gtf::scdr::ShaderConstant* offsets;
    gtf::scdr::ShaderConstant* weights;
    gtf::scdr::ShaderConstant* weightCount;
    gtf::scdr::ShaderConstant* direction;
};

struct BlitPassShaderConstants
{
    gtf::scdr::ShaderConstant* projMat;
    gtf::scdr::ShaderConstant* modelMat;
    gtf::scdr::ShaderConstant* object2Alpha;
    gtf::scdr::ShaderConstant* texture0;
    gtf::scdr::ShaderConstant* texture1;
};

struct GaussianBlurGraphicObjects
{
    BlurPassShaderConstants blurShaderConstants;
    BlitPassShaderConstants shaderConstants;
    BlitPassShaderConstants shaderConstantsWithAlphaMask;

    gtf::scdr::PipelineStateObject* psoBlur;
    gtf::scdr::PipelineStateObject* psoBlit;
    gtf::scdr::PipelineStateObject* psoAlphaBlit;
    gtf::scdr::VertexBuffer*        vertices;
    gtf::scdr::SamplerState*        sampler0;
    gtf::scdr::SamplerState*        sampler1;
};

//!
//! \brief Implements a Gaussian blur widget. The blur effect is applied to all child widgets.
//!
class GaussianBlur : public gtf::scdr::IWidgetRenderer
{
private:
    GaussianBlur(const GaussianBlur&);
    GaussianBlur& operator=(const GaussianBlur&);

    GaussianBlur(const gtf::scene::TypeResolver& resolver_);

public:
    //!
    //! \brief Creates an instance of the GaussianBlur renderer.
    //!
    //! \param[in] resolver_ The type resolver.
    //! \param[in] dataStore_ The DataStore that provides access to various other SCDR interfaces.
    //! \param[in] commandBuffer_ The CommandBuffer for creating GPU resources.
    //! \param[in] frameAllocator_ An efficient allocator for transient, short-lived memory that can be used during command execution.
    //! \param[in] createParams_ A pointer to user specific parameters.
    //!
    //! \return An instance of the GaussianBlur.
    //!
    //! \note
    //!   The lifetime of the createParams_ must extend over the entire lifetime of the program. Also, the
    //!   instance will be destroyed through destroyInstance on shutdown.
    //!
    static gtf::scdr::IWidgetRenderer* createInstance(
        const gtf::scene::TypeResolver& resolver_,
        gtf::scdr::DataStore&           dataStore_,
        gtf::scdr::CommandBuffer&       commandBuffer_,
        gtf::scdr::Allocator&           frameAllocator_,
        const void*                     createParams_);

    //!
    //! \brief Destroys an instance of the RippleEffect
    //!
    //! \param[in] renderer_ The renderer to destroy.
    //! \param[in] dataStore_ The DataStore to provide access to various other SCDR interfaces.
    //! \param[in] commandBuffer_ The CommandBuffer for destroying GPU resources.
    //! \param[in] frameAllocator_ An efficient allocator for transient, short-lived memory that can be used during command execution.
    //!
    static void destroyInstance(
        gtf::scdr::IWidgetRenderer* renderer_,
        gtf::scdr::DataStore&       dataStore_,
        gtf::scdr::CommandBuffer&   commandBuffer_,
        gtf::scdr::Allocator&       frameAllocator_);

    ~GaussianBlur() = default;

public: // gtf::scdr::WidgetRenderer interface
    bool init(
        const gtf::scene::TypeResolver& typeResolver_,
        gtf::scdr::DataStore&           dataStore_,
        gtf::scdr::CommandBuffer&       commandBuffer_,
        gtf::scdr::Allocator&           frameAllocator_) final override;

    void deinit(
        gtf::scdr::DataStore&     dataStore_,
        gtf::scdr::CommandBuffer& commandBuffer_,
        gtf::scdr::Allocator&     frameAllocator_) final override;

    bool update(
        gtf::scdr::ResourceLoader& resourceLoader_,
        gtf::scdr::DataStore&      dataStore_,
        gtf::scdr::CommandBuffer&  commandBuffer_,
        gtf::scdr::Allocator&      frameAllocator_) final override;

    bool render(
        gtf::scdr::DataStore&     dataStore_,
        gtf::scdr::CommandBuffer& commandBuffer_,
        gtf::scdr::Allocator&     frameAllocator_) final override;

    bool begin(
        gtf::scdr::DataStore&     dataStore_,
        gtf::scdr::CommandBuffer& commandBuffer_,
        gtf::scdr::Allocator&     frameAllocator_) final override;

    bool end(
        gtf::scdr::DataStore&     dataStore_,
        gtf::scdr::CommandBuffer& commandBuffer_,
        gtf::scdr::Allocator&     frameAllocator_) final override;

    gtf::scdr::RenderObject* createRenderObject(
        gtf::properties::Container& widget_,
        gtf::scdr::DataStore&       dataStore_,
        gtf::scdr::CommandBuffer&   commandBuffer_,
        gtf::scdr::Allocator&       frameAllocator_) final override;

    void destroyRenderObject(
        gtf::scdr::RenderObject*  object_,
        gtf::scdr::DataStore&     dataStore_,
        gtf::scdr::CommandBuffer& commandBuffer_,
        gtf::scdr::Allocator&     frameAllocator_) final override;

    bool traverse(
        gtf::properties::Container&              widget_,
        const gtf::scdr::RenderObjectStackFrame& parentStackFrame_,
        gtf::scdr::RenderObjectStackFrame&       objectStackFrame_,
        gtf::scdr::RenderObject&                 object_,
        gtf::scdr::TraverserContext&             traverserContext_) final override;

private:
    bool isValid() const;
    bool enable(bool enabledPropety_) const;

private:
    gtf::scdr::GraphicsApi     api;
    GaussianBlurRenderObject*  objects;
    GaussianBlurGraphicObjects graphicObjects;
    GaussianBlurProperties     properties;
    GaussianBlurFeatures       features;
};

#endif // GAUSSIAN_BLUR_H
