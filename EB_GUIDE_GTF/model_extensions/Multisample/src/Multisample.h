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

#ifndef MULTISAMPLE_RENDERER_H
#define MULTISAMPLE_RENDERER_H

#include <gtf/scdr/gpu/ShaderConstant.h>
#include <gtf/scdr/widgets/IWidgetRenderer.h>
#include <gtf/scdr/RenderObjectUtils.h>

struct MultisampleVertex
{
    float x;
    float y;
};

struct MultisampleVertexWithUV
{
    float posX;
    float posY;
    float texU;
    float texV;
};

struct MultisampleShaderConstants
{
    struct
    {
        gtf::scdr::ShaderConstant* projMat;
        gtf::scdr::ShaderConstant* modelMat;
        gtf::scdr::ShaderConstant* color;
    } offscreenPass;

    struct
    {
        gtf::scdr::ShaderConstant* projMat;
        gtf::scdr::ShaderConstant* modelMat;
        gtf::scdr::ShaderConstant* texture;
    } defaultPass;

    struct
    {
        gtf::scdr::ShaderConstant* projMat;
        gtf::scdr::ShaderConstant* modelMat;
        gtf::scdr::ShaderConstant* texture;
        gtf::scdr::ShaderConstant* object2Alpha;
        gtf::scdr::ShaderConstant* textureAlphaMask;
    } defaultPassWithAlphaMask;
};

struct MultisamplePipelineStateObjects
{
    gtf::scdr::PipelineStateObject* offscreenPSO;
    gtf::scdr::PipelineStateObject* defaultPSO;
    gtf::scdr::PipelineStateObject* defaultPSOWithAlphaMask;
};

struct MultisamplePropertyHandles
{
    gtf::scdr::WidgetPropertiesCommon2D common2d;
    uint16_t                            fillColor;
    uint16_t                            sampleCount;
    uint16_t                            animatedProperty;
};

struct MultisampleGraphicObjects
{
    MultisampleShaderConstants          shaderConstants;
    MultisamplePipelineStateObjects     psos;
    gtf::scdr::VertexBuffer* vertexBuffer;
    gtf::scdr::VertexBuffer* compositingQuadVertexBuffer;
    gtf::scdr::SamplerState* sampler;
};

class MultisampleRenderObject;
class MultisampleStackFrame;

//!
//! \brief This is the implementation of a custom renderer.
//!
class Renderer : public gtf::scdr::IWidgetRenderer
{
private:
    static const char* ClassName;
    Renderer(const Renderer&);
    Renderer& operator=(const Renderer&);
    Renderer(const gtf::scene::TypeResolver& resolver_, gtf::scdr::GraphicsApi api_);

public:
    //!
    //! \brief Creates an instance of the custom renderer.
    //!
    //! \param[in] resolver_ The type resolver.
    //! \param[in] dataStore_ The DataStore that provides access to various other SCDR interfaces.
    //! \param[in] commandBuffer_ The CommandBuffer for creating GPU resources.
    //! \param[in] allocator_ An efficient allocator for transient, short-lived memory that can be used during command execution.
    //! \param[in] createParams_ A pointer to user specific parameters.
    //!
    //! \return An instance of the custom renderer.
    //!
    //! \note The lifetime of the createParams_ must extend over the entire lifetime of the program.
    //!
    static gtf::scdr::IWidgetRenderer* createInstance(
        const gtf::scene::TypeResolver& resolver_,
        gtf::scdr::DataStore&           dataStore_,
        gtf::scdr::CommandBuffer&       commandBuffer_,
        gtf::scdr::Allocator&           allocator_,
        const void*                     createParams_);

    //!
    //! \brief Destroys an instance of the custom renderer.
    //!
    //! \param[in] renderer_ The renderer to destroy.
    //! \param[in] dataStore_ The DataStore to provide access to various other SCDR interfaces.
    //! \param[in] commandBuffer_ The CommandBuffer for destroying GPU resources.
    //! \param[in] allocator_ An efficient allocator for transient, short-lived memory that can be used during command execution.
    //!
    //!
    static void destroyInstance(
        gtf::scdr::IWidgetRenderer* renderer_,
        gtf::scdr::DataStore&       dataStore_,
        gtf::scdr::CommandBuffer&   commandBuffer_,
        gtf::scdr::Allocator&       allocator_);

    virtual ~Renderer() {};

    // gtf::scdr::IWidgetRenderer interface
    virtual bool init(
        const gtf::scene::TypeResolver& typeResolver_,
        gtf::scdr::DataStore&           dataStore_,
        gtf::scdr::CommandBuffer&       cmdBuffer_,
        gtf::scdr::Allocator&           frameAllocator_) final override;

    virtual void deinit(
        gtf::scdr::DataStore&     dataStore_,
        gtf::scdr::CommandBuffer& commandBuffer_,
        gtf::scdr::Allocator&     frameAlloactor_) final override;

    virtual gtf::scdr::RenderObject* createRenderObject(
        gtf::properties::Container& widget_,
        gtf::scdr::DataStore&       dataStore_,
        gtf::scdr::CommandBuffer&   commandBuffer_,
        gtf::scdr::Allocator&       frameAllocator_) final override;

    virtual void destroyRenderObject(
        gtf::scdr::RenderObject*  object_,
        gtf::scdr::DataStore&     dataStore_,
        gtf::scdr::CommandBuffer& commandBuffer_,
        gtf::scdr::Allocator&     frameAllocator_) final override;

    virtual bool traverse(
        gtf::properties::Container&              widget_,
        const gtf::scdr::RenderObjectStackFrame& parentStackFrame_,
        gtf::scdr::RenderObjectStackFrame&       objectStackFrame_,
        gtf::scdr::RenderObject&                 object_,
        gtf::scdr::TraverserContext&             tc_) final override;

    virtual bool update(
        gtf::scdr::ResourceLoader& resourceLoader_,
        gtf::scdr::DataStore&      dataStore_,
        gtf::scdr::CommandBuffer&  commandBuffer_,
        gtf::scdr::Allocator&      frameAllocator_) final override;

    virtual bool render(
        gtf::scdr::DataStore&     dataStore_,
        gtf::scdr::CommandBuffer& commandBuffer_,
        gtf::scdr::Allocator&     frameAllocator_) final override;

    virtual bool begin(
        gtf::scdr::DataStore&     dataStore_,
        gtf::scdr::CommandBuffer& commandBuffer_,
        gtf::scdr::Allocator&     frameAllocator_) final override;

    virtual bool end(
        gtf::scdr::DataStore&     dataStore_,
        gtf::scdr::CommandBuffer& commandBuffer_,
        gtf::scdr::Allocator&     frameAllocator_) final override;

private:

    // Some variables for the generated particles
    static const uint32_t gridSideCellCount = 10;
    static const uint32_t cellLength = 120;
    MultisampleVertex     vertices[(gridSideCellCount + 1) * gridSideCellCount * 2 * 3];

    gtf::scdr::GraphicsApi            api;
    MultisampleGraphicObjects         graphicObjects;
    MultisampleRenderObject*          objects;
    MultisamplePropertyHandles        properties;
    gtf::scdr::WidgetFeaturesCommon2D features;
    float                             animatedProperty;
};

#endif // MULTISAMPLE_RENDERER_H 
