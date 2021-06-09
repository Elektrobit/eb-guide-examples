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

#include <gtf/types/Macros.h>
#include <gtf/scdr/SDK.h>

#include "Renderer.h"
#include "RenderObject.h"
#include "RenderObjectStackFrame.h"

const char* extendedcontainerwidget::Renderer::ClassName = "GtfExtendedContainerWidget";

gtf::scdr::IWidgetRenderer* extendedcontainerwidget::Renderer::createInstance(
    const gtf::scene::TypeResolver& resolver_,
    gtf::scdr::DataStore&           dataStore_,
    gtf::scdr::CommandBuffer&,
    gtf::scdr::Allocator&,
    const void*)
{
    BIND_ALLOCATOR(allocator, dataStore_);

    return ALLOC_OBJECT(allocator, extendedcontainerwidget::Renderer, resolver_, dataStore_.getSystemConfig().getGraphicsApi());
}

void extendedcontainerwidget::Renderer::destroyInstance(
    gtf::scdr::IWidgetRenderer* renderer_,
    gtf::scdr::DataStore&       dataStore_,
    gtf::scdr::CommandBuffer&,
    gtf::scdr::Allocator&)
{
    BIND_ALLOCATOR(allocator, dataStore_);

    return FREE_OBJECT(allocator, static_cast<extendedcontainerwidget::Renderer*>(renderer_));
}

extendedcontainerwidget::Renderer::Renderer(const gtf::scene::TypeResolver& resolver_, gtf::scdr::GraphicsApi api_)
    : api(api_)
{
    // Resolves the widget's default handles
    gtf::scdr::ResolvePropertiesCommon2D(resolver_, ClassName, properties.common2d);
    gtf::scdr::ResolveFeaturesCommon2D(resolver_, features);
}

extendedcontainerwidget::Renderer::~Renderer()
{
}

bool extendedcontainerwidget::Renderer::init(const gtf::scene::TypeResolver&, gtf::scdr::DataStore&, gtf::scdr::CommandBuffer&, gtf::scdr::Allocator&)
{
    return true;
}

void extendedcontainerwidget::Renderer::deinit(gtf::scdr::DataStore&, gtf::scdr::CommandBuffer&, gtf::scdr::Allocator&)
{
}

bool extendedcontainerwidget::Renderer::begin(gtf::scdr::DataStore&, gtf::scdr::CommandBuffer&, gtf::scdr::Allocator&)
{
    return true;
}

bool extendedcontainerwidget::Renderer::traverse(
    gtf::properties::Container&              widget_,
    const gtf::scdr::RenderObjectStackFrame& parentStackFrame_,
    gtf::scdr::RenderObjectStackFrame&       objectStackFrame_,
    gtf::scdr::RenderObject&                 object_,
    gtf::scdr::TraverserContext&)
{
    RenderObject&                              object = static_cast<RenderObject&>(object_);
    RenderObjectStackFrame&                    frame  = static_cast<RenderObjectStackFrame&>(objectStackFrame_);
    const gtf::scdr::RenderObjectStackFrame2D& parent = static_cast<const gtf::scdr::RenderObjectStackFrame2D&>(parentStackFrame_);

    gtf::scdr::WidgetPropertiesCommon2DValues propertyValues;
    gtf::scdr::RenderObjectStackFrame2DValues parent2d;
    gtf::scdr::WidgetFeaturesCommon2DValues   featureValues;

    // Get widget properties like x, y, width, height, ... from the widget model
    if (!gtf::scdr::GetWidgetPropertiesCommon2DValues(widget_, properties.common2d, propertyValues))
    {
        return false;
    }

    // Get widget feature properties like translation, scaling, ... from the widget model
    gtf::scdr::GetWidgetFeaturesCommon2DValues(widget_, object.features, features, featureValues);
    gtf::scdr::GetRenderObjectStackFrameValues2D(parent, parent2d);

    // Calculate the transformation matrices for this widget
    gtf::scdr::WidgetTransformations transforms;
    gtf::scdr::CalculateWidgetTransformations2D(parent2d, propertyValues, featureValues, transforms);

    const float fWidth  = static_cast<float>(propertyValues.width);
    const float fHeight = static_cast<float>(propertyValues.height);

    // Calculate the scissor rectangle
    gtf::scdr::CalculateScissorRectangle(
        featureValues,
        parent2d,
        transforms.widget2Framebuffer,
        fWidth,
        fHeight,
        frame.scissor);

    // Calculate the coloration
    gtf::scdr::CalculateColoration(featureValues, parent2d, frame.coloration);

    // Set the values for the child widgets of this widget
    frame.framebuffer        = parent2d.framebuffer;
    frame.framebufferId      = parent2d.framebufferId;
    frame.viewProjMat        = parent2d.viewProjMat;
    frame.viewport           = parent2d.viewport;
    frame.framebuffer2Screen = parent2d.framebuffer2Screen;
    frame.depth              = parent2d.depth;
    frame.alphaMask          = parent2d.alphaMask;
    frame.widget2AlphaMask   = transforms.widget2AlphaMask;
    frame.widget2Framebuffer = transforms.widget2StackFrame;

    // Set the values for this widget, in this case they are used for touch.
    object.width         = fWidth;
    object.height        = fHeight;
    object.widget2Screen = transforms.widget2Screen;
    object.scissor       = frame.scissor;

    return true;
}

gtf::scdr::RenderObject* extendedcontainerwidget::Renderer::createRenderObject(
    gtf::properties::Container& widget_,
    gtf::scdr::DataStore&       dataStore_,
    gtf::scdr::CommandBuffer&,
    gtf::scdr::Allocator&)
{
    BIND_ALLOCATOR(allocator, dataStore_);

    return ALLOC_OBJECT(allocator, RenderObject, widget_, features);
}

void extendedcontainerwidget::Renderer::destroyRenderObject(
    gtf::scdr::RenderObject* object_,
    gtf::scdr::DataStore&    dataStore_,
    gtf::scdr::CommandBuffer&,
    gtf::scdr::Allocator&)
{
    BIND_ALLOCATOR(allocator, dataStore_);

    FREE_OBJECT(allocator, static_cast<RenderObject*>(object_));
}

bool extendedcontainerwidget::Renderer::update(gtf::scdr::ResourceLoader&, gtf::scdr::DataStore&, gtf::scdr::CommandBuffer&, gtf::scdr::Allocator&)
{
    // A container widget does not render anything. Therefore nothing to do here.
    return true;
}

bool extendedcontainerwidget::Renderer::render(gtf::scdr::DataStore&, gtf::scdr::CommandBuffer&, gtf::scdr::Allocator&)
{
    // A container widget does not render anything. Therefore nothing to do here.
    return true;
}

bool extendedcontainerwidget::Renderer::end(gtf::scdr::DataStore&, gtf::scdr::CommandBuffer&, gtf::scdr::Allocator&)
{
    return true;
}
