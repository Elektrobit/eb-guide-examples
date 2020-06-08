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

#include <gtf/metainformation/WidgetDescriptorMacros.h>

#include <gtf/typesystem/TypeManager.h>

#include "MultisampleDesc.h"
#include "LoadedStageImpl.h"

namespace multisample
{
    const char* WidgetDesc::WidgetName = "Multisample";

    // pre-defined property types
    static const gtf::type::TypeBase s_typeInt32("int32_t");               // int32_t property
    static const gtf::type::TypeBase typeFloat("float");                   // float property
    static const gtf::type::TypeList typeVec3(typeFloat);                  // vector of 3 int32_t property
    static const gtf::type::TypeList typeVec4(typeFloat);                  // vector of 4 int32_t property
    static const gtf::type::TypeBase typeString("string");                 // string property
    static const gtf::type::TypeTypedef s_typeColor("color", s_typeInt32); // color property. Base type is int32_t
    static const gtf::type::TypeResource s_typeImage("image");             // image property as resource type

    // Additional properties of the custom widget. Default properties like x, y,
    // width, height, isVisible are already defined in GtfAbstractVisualWidget
    static gtf::metainformation::PropertyDescriptor MultisampleProperties[] =
    {
        gtf::metainformation::PropertyDescriptor(&s_typeColor                                                           // property type
        , "fillColor"                                                                                                   // property name
        , "The color"                                                                                                   // property description
        , "Common"                                                                                                      // property category
        , "0,0,0,255"),                                                                                                 // default value

        gtf::metainformation::PropertyDescriptor(&s_typeInt32                                                           // property type
        , "sampleCount"                                                                                                 // property name
        , "The number of samples for multisampling, it should be between 1 and the maximum count of samples supported"  // property description
        , "Common"                                                                                                      // property category
        , "1"),                                                                                                         // default value

        gtf::metainformation::PropertyDescriptor(&typeFloat                                                             // property type
        , "animatedProperty"                                                                                            // property name
        , "The property used for particle animation [0-1]"                                                              // property description
        , "Common"                                                                                                      // property category
        , "0")                                                                                                          // default value
    };

    // The widget descriptor
    static const gtf::metainformation::WidgetDescriptor widget_desc[] =
    {
        FULL_WIDGET(WidgetDesc::WidgetName                  // widget name
        , "Multisample"                                     // widget Alias
        , "A custom widget drawing multisampled geometry."  // widget description
        , "Custom widgets"                                  // widget set name (category in EB GUIDE Studio)
        , "GtfAbstractVisualWidget"                         // base widget
        , false                                             // is abstract (abstract widget can not be used in EB GUIDE Studio)
        , 0x0100                                            // version, here 1.0
        , false                                             // is view
        , true                                              // can have children
        , false                                             // is instantiator
        , MultisampleProperties                             // array of widget properties
        , NULL)
    };

    gtf::metainformation::ActionDescriptor const* WidgetDesc::GetActions(uint32_t& count_) const
    {
        count_ = 0;
        return NULL;
    }

    gtf::metainformation::PopupStackDescriptor const* WidgetDesc::GetPopupStacks(uint32_t& count_) const
    {
        count_ = 0;
        return NULL;
    }

    gtf::metainformation::WidgetDescriptor const* WidgetDesc::GetWidgets(uint32_t& count_) const
    {
        count_ = ARRAY_SIZE(widget_desc);
        return widget_desc;
    }

    gtf::metainformation::WidgetFeatureDescriptor const* WidgetDesc::GetWidgetFeatures(uint32_t& count_) const
    {
        count_ = 0;
        return NULL;
    }

    gtf::metainformation::ResourceDescriptor const* WidgetDesc::GetResourceTypes(uint32_t& count_) const
    {
        count_ = 0;
        return NULL;
    }

    // Function to resolve the types used by the custom properties
    bool WidgetDesc::ResolveTypes(gtf::typesystem::TypeManager const* typeManager)
    {
        bool success = true;

        RESOLVE_PROPERTIES(MultisampleProperties);

        return success;
    }
}
