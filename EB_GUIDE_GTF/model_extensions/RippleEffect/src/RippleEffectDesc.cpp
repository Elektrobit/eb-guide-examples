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

#include "RippleEffectDesc.h"

const char* RippleEffectDesc::WidgetName = "RippleEffect";

// pre-defined property types
static const gtf::type::TypeBase s_typeInt32("int32_t");               // int32_t property
static const gtf::type::TypeBase typeFloat("float");                   // float property
static const gtf::type::TypeList typeVec3(typeFloat);                  // vector of 3 int32_t property
static const gtf::type::TypeList typeVec4(typeFloat);                  // vector of 4 int32_t property
static const gtf::type::TypeBase typeString("string");                 // string property
static const gtf::type::TypeTypedef s_typeColor("color", s_typeInt32); // color property. Base type is int32_t
static const gtf::type::TypeResource s_typeImage("image");             // image property as resource type
static const gtf::type::TypeBase s_typeBool("bool");                   // bool property

// Additional properties of the custom widget. Default properties like x, y,
// width, height, isVisible are already defined in GtfAbstractVisualWidget
static gtf::metainformation::PropertyDescriptor RippleEffectProperties[] =
{
    gtf::metainformation::PropertyDescriptor(&s_typeInt32          // property type
    , "animationTime"                        // property name
    , "The time of the animation"            // property description
    , "Common"                               // property category
    , "0"),                                  // default value
    gtf::metainformation::PropertyDescriptor(&s_typeBool           // property type
    , "enabled"                              // property name
    , "True if the ripple effect is enabled" // property description
    , "Common"                               // property category
    , "true"),                               // default value
};

// The widget descriptor
static const gtf::metainformation::WidgetDescriptor widgets[] =
{
    FULL_WIDGET(RippleEffectDesc::WidgetName    // widget name
    , "Ripple effect"                           // widget Alias
    , "A custom widget adding a ripple effect." // widget description
    , "Custom widgets"                          // widget set name (category in EB GUIDE Studio)
    , "GtfAbstractVisualWidget"                 // base widget
    , false                                     // is abstract (abstract widget can not be used in EB GUIDE Studio)
    , 0x0100                                    // version, here 1.0
    , false                                     // is view
    , true                                      // can have children
    , false                                     // is instantiator
    , RippleEffectProperties                    // array of widget properties
    , NULL),
};

gtf::metainformation::ActionDescriptor const* RippleEffectDesc::GetActions(uint32_t& count_) const
{
    count_ = 0;
    return NULL;
}

gtf::metainformation::PopupStackDescriptor const* RippleEffectDesc::GetPopupStacks(uint32_t& count_) const
{
    count_ = 0;
    return NULL;
}

gtf::metainformation::WidgetDescriptor const* RippleEffectDesc::GetWidgets(uint32_t& count_) const
{
    count_ = ARRAY_SIZE(widgets);
    return widgets;
}

gtf::metainformation::WidgetFeatureDescriptor const* RippleEffectDesc::GetWidgetFeatures(uint32_t& count_) const
{
    count_ = 0;
    return NULL;
}

gtf::metainformation::ResourceDescriptor const* RippleEffectDesc::GetResourceTypes(uint32_t& count_) const
{
    count_ = 0;
    return NULL;
}

// Function to resolve the types used by the custom properties
bool RippleEffectDesc::ResolveTypes(gtf::typesystem::TypeManager const* typeManager_)
{
    bool success = true;
    for (uint32_t propertyTypeIndex = 0; propertyTypeIndex < ARRAY_SIZE(RippleEffectProperties); ++propertyTypeIndex)
    {
        gtf::metainformation::PropertyDescriptor *curProperty = &(RippleEffectProperties[propertyTypeIndex]);
        curProperty->m_pType = typeManager_->getTypeByType(curProperty->m_pUnresolvedType);
        success = success && curProperty->m_pType;
    }
    return success;
}
