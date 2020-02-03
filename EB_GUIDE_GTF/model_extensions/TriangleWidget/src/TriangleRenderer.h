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

#ifndef CUSTOM_WIDGET_TRIANGLE_RENDERER_H_INCLUDED
#define CUSTOM_WIDGET_TRIANGLE_RENDERER_H_INCLUDED

#include <gtf/scdr/RenderObject.h>
#include <gtf/scdr/RenderObjectUtils.h>
#include <gtf/scdr/gpu/ShaderConstant.h>
#include <gtf/scdr/widgets/IWidgetRenderer.h>

struct TriangleShaderConstants
{
    gtf::scdr::ShaderConstant* projMat;
    gtf::scdr::ShaderConstant* modelMat;
    gtf::scdr::ShaderConstant* color;
    gtf::scdr::ShaderConstant* object2Alpha;
    gtf::scdr::ShaderConstant* textureAlphaMask;
};

struct TrianglePropertyHandles
{
    gtf::scdr::WidgetPropertiesCommon2D common2d;
    uint16_t                            fillColor;
};

class TriangleRenderObject;
class TriangleRenderObjectStackFrame;

//!
//! \brief This is the implementation of a custom renderer using the OpenGL|ES 2.0 API
//!
class TriangleRenderer : public gtf::scdr::IWidgetRenderer
{
private:
    static const char* ClassName;
    TriangleRenderer(TriangleRenderer&);
    TriangleRenderer& operator=(const TriangleRenderer&);

public:
    //!
    //! \brief Creates an instance of the custom renderer.
    //!
    //! \param[in] context_ The SCDR graphics context.
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
    //! \param[in] context_ The SCDR graphics context.
    //!
    //!
    static void destroyInstance(
        gtf::scdr::IWidgetRenderer* renderer_,
        gtf::scdr::DataStore&       dataStore_,
        gtf::scdr::CommandBuffer&   commandBuffer_,
        gtf::scdr::Allocator&       allocator_);

    virtual ~TriangleRenderer();

public: // gtf::scdr::IWidgetRenderer interface
    virtual bool             init(const gtf::scene::TypeResolver& typeResolver_, gtf::scdr::DataStore& dataStore_, gtf::scdr::CommandBuffer& commandBuffer_, gtf::scdr::Allocator& frameAllocator_);
    virtual void             deinit(gtf::scdr::DataStore& dataStore_, gtf::scdr::CommandBuffer& commandBuffer_, gtf::scdr::Allocator& frameAlloactor_);
    gtf::scdr::RenderObject* createRenderObject(gtf::properties::Container& widget_, gtf::scdr::DataStore& dataStore_, gtf::scdr::CommandBuffer& commandBuffer_, gtf::scdr::Allocator& frameAllocator_);
    virtual void             destroyRenderObject(gtf::scdr::RenderObject* object_, gtf::scdr::DataStore& dataStore_, gtf::scdr::CommandBuffer& commandBuffer_, gtf::scdr::Allocator& frameAllocator_);
    virtual bool             traverse(gtf::properties::Container& widget_, const gtf::scdr::RenderObjectStackFrame& parentStackFrame_, gtf::scdr::RenderObjectStackFrame& objectStackFrame_, gtf::scdr::RenderObject& object_, gtf::scdr::TraverserContext& traverserContext_);
    virtual bool             update(gtf::scdr::ResourceLoader& resourceLoader_, gtf::scdr::DataStore& dataStore_, gtf::scdr::CommandBuffer& commandBuffer_, gtf::scdr::Allocator& frameAllocator_);
    virtual bool             render(gtf::scdr::DataStore& dataStore_, gtf::scdr::CommandBuffer& commandBuffer_, gtf::scdr::Allocator& frameAllocator_);
    virtual bool             begin(gtf::scdr::DataStore& dataStore_, gtf::scdr::CommandBuffer& commandBuffer_, gtf::scdr::Allocator& frameAllocator_);
    virtual bool             end(gtf::scdr::DataStore& dataStore_, gtf::scdr::CommandBuffer& commandBuffer_, gtf::scdr::Allocator& frameAllocator_);

private:
    enum RenderMode
    {
        RenderModeOpaque,
        RenderModeTransparent,
        RenderModeCount
    };

private:
    gtf::scdr::ShaderProgram* createShaderProgram(gtf::scdr::GPUContext& gc_, gtf::scdr::DataStore& dataStore_, gtf::scdr::CommandBuffer& commandBuffer_) const;
    gtf::scdr::ShaderProgram* createShaderProgramWithAlphaMask(gtf::scdr::GPUContext& gc_, gtf::scdr::DataStore& dataStore_, gtf::scdr::CommandBuffer& commandBuffer_) const;
    bool                      createPipelineStateObject(RenderMode mode_, gtf::scdr::ShaderProgram* program_, gtf::scdr::GPUContext& gc_, gtf::scdr::CommandBuffer& commandBuffer_, gtf::scdr::PipelineStateObject*& psoOut_) const;
    bool                      createVertexBuffer(gtf::scdr::GPUContext& gc_, gtf::scdr::CommandBuffer& commandBuffer_, gtf::scdr::VertexBuffer*& vertexBufferOut_) const;

private:
    static void updateTransformations(
        const gtf::scdr::RenderObjectStackFrame2DValues& stackValues_,
        const gtf::scdr::WidgetPropertiesCommon2DValues& properties_,
        const gtf::scdr::WidgetFeaturesCommon2DValues&   features_,
        TriangleRenderObject&                            object_,
        TriangleRenderObjectStackFrame&                  objectStack_);

    gtf::scdr::Color getFillColor(gtf::properties::Container& widget_) const;

    TriangleRenderer(const gtf::scene::TypeResolver& resolver_, gtf::scdr::GraphicsApi api_);

private:
    gtf::scdr::PipelineStateObject* pipelineStateObject[RenderModeCount];
    gtf::scdr::PipelineStateObject* pipelineStateObjectWithAlphaMask[RenderModeCount];
    TriangleShaderConstants         shaderConstants;
    TriangleShaderConstants         shaderConstantsWithAlphaMask;
    gtf::scdr::VertexBuffer*        vertexBuffer;
    gtf::scdr::VertexBuffer*        vertexBufferWithAlphaMask;

    // Linked list for storing triangle render objects
    TriangleRenderObject* objects;

public:
    TrianglePropertyHandles           properties;
    gtf::scdr::WidgetFeaturesCommon2D features;
    gtf::scdr::GraphicsApi            api;
};

#endif // CUSTOM_WIDGET_TRIANGLE_RENDERER_H_INCLUDED
