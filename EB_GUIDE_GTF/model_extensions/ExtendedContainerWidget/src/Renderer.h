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

#ifndef GTF_EXTENDED_CONTAINER_RENDERER_H_INCLUDED
#define GTF_EXTENDED_CONTAINER_RENDERER_H_INCLUDED

#include <gtf/scdr/RenderObject.h>
#include <gtf/scdr/RenderObjectUtils.h>
#include <gtf/scdr/widgets/IWidgetRenderer.h>

namespace extendedcontainerwidget {
struct PropertyHandles
{
    gtf::scdr::WidgetPropertiesCommon2D common2d;
};

//!
//! \brief This is the implementation of a custom renderer using the OpenGL|ES 2.0 API
//!
class Renderer : public gtf::scdr::IWidgetRenderer
{
private:
    static const char* ClassName;
    Renderer(Renderer&);
    Renderer& operator=(const Renderer&);

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
        gtf::scdr::Allocator&           frameAllocator_,
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
        gtf::scdr::Allocator&       frameAllocator_);

    virtual ~Renderer();

public: // gtf::scdr::WidgetRenderer interface
    virtual bool init(const gtf::scene::TypeResolver& typeResolver_, gtf::scdr::DataStore& dataStore_, gtf::scdr::CommandBuffer& commandBuffer_, gtf::scdr::Allocator& frameAllocator_);
    virtual void deinit(gtf::scdr::DataStore& dataStore_, gtf::scdr::CommandBuffer& commandBuffer_, gtf::scdr::Allocator& frameAllocator_);

    virtual bool update(gtf::scdr::ResourceLoader& resourceLoader_, gtf::scdr::DataStore& dataStore_, gtf::scdr::CommandBuffer& commandBuffer_, gtf::scdr::Allocator& frameAllocator_);

    virtual bool render(gtf::scdr::DataStore& dataStore_, gtf::scdr::CommandBuffer& commandBuffer_, gtf::scdr::Allocator& frameAllocator_);

    virtual bool begin(gtf::scdr::DataStore& dataStore_, gtf::scdr::CommandBuffer& commandBuffer_, gtf::scdr::Allocator& frameAllocator_);
    virtual bool end(gtf::scdr::DataStore& dataStore_, gtf::scdr::CommandBuffer& commandBuffer_, gtf::scdr::Allocator& frameAllocator_);

    virtual gtf::scdr::RenderObject* createRenderObject(
        gtf::properties::Container& widget_,
        gtf::scdr::DataStore&       dataStore_,
        gtf::scdr::CommandBuffer&   commandBuffer_,
        gtf::scdr::Allocator&       frameAllocator_);

    virtual void destroyRenderObject(
        gtf::scdr::RenderObject*  object_,
        gtf::scdr::DataStore&     dataStore_,
        gtf::scdr::CommandBuffer& commandBuffer_,
        gtf::scdr::Allocator&     frameAllocator_);

    virtual bool traverse(
        gtf::properties::Container&              widget_,
        const gtf::scdr::RenderObjectStackFrame& parentStackFrame_,
        gtf::scdr::RenderObjectStackFrame&       objectStackFrame_,
        gtf::scdr::RenderObject&                 object_,
        gtf::scdr::TraverserContext&             traverserContext_);

private:
    Renderer(const gtf::scene::TypeResolver& resolver_, gtf::scdr::GraphicsApi api_);

public:
    PropertyHandles                   properties;
    gtf::scdr::WidgetFeaturesCommon2D features;
    gtf::scdr::GraphicsApi            api;
};
} // namespace extendedcontainerwidget
#endif // GTF_EXTENDED_CONTAINER_RENDERER_H_INCLUDED
