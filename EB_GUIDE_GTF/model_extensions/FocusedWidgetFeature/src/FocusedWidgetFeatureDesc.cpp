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
#include "FocusedWidgetFeatureDesc.h"

// These are property definitions which can be used in the widget, widget feature and action definitions.
static const gtf::type::TypeBase typeVoid("void");
static const gtf::type::TypeBase typeString("string");
static const gtf::type::TypeBase typeBool("bool");
static const gtf::type::TypeBase typeInt32("int32_t");

static const gtf::type::TypeTypedef typeColor("color", typeInt32);
static const gtf::type::TypeResource typeFont("font");
static const gtf::type::TypeResource typeImage("image");
static const gtf::type::TypeResource typeModel("model");

static const gtf::type::TypeList typeListInt32(typeInt32);
static const gtf::type::TypeList typeListBool(typeBool);

static const gtf::type::TypeList typeListString(typeString);
static const gtf::type::TypeList typeListColor(typeColor);
static const gtf::type::TypeList typeListFont(typeFont);
static const gtf::type::TypeList typeListImage(typeImage);
static const gtf::type::TypeList typeListModel(typeModel);

/* The definition of a property for the property array definitions is like the following :
*      ~ PropertyType (like defined above)
*      ~ Name of the property (the name displayed for the widget variable within guide)
*      ~ Description of the property (the description displayed for the widget variable within guide)
*      ~ Category of the property (the category used for property grouping within the widget variable component)
*      ~ Default value (the default value for the widget variable as STRING)
*      ~ Size of the constant value definitions array (see Constant values definition)
*      ~ Constant value definitions array (The constant values are used as some kind of enumeration within EB GUIDE Studio.
*      E.g. the integer property "align" has the constants "left, center, right")
*
*    The property constant descriptor consists of the following information :
*      ~ Constant name (name of the constant which is displayed as enum name for the property)
*      ~ Constant value (the value associated with the constants name)
*
*
* example :
*   static PropertyConstantDescriptor NAME_OF_THE_PROPERTY_CONSTANT_ARRAY[] =
*   {
*     { "SampleText1"
*     , "My sample text 1"},
*     { "SampleText2"
*     , "My sample text 2"},
*     { "SampleText3"
*     , "My sample text 3"}
*   };
*
*   static PropertyDescriptor NAME_OF_THE_PROPERTY_ARRAY[] =
*   {
*     PropertyDescriptor(&typeString                // property type
*     , "TestDescription"                           // property name
*     , "Optional Test Case Description"            // property description
*     , "Test"                                      // property category
*     , "This is a test"                            // property default value
*     , ARRAY_SIZE(NAME_OF_THE_PROPERTY_CONSTANT_ARRAY) // property constant value definitions array size
*     , NAME_OF_THE_PROPERTY_CONSTANT_ARRAY         // property constant definitions array
*     )
*   };
*/

/*\brief The definition of a script function.
*
*   A script function array consists of a number of action descriptors.
*   An action descriptor has the following attributes :
*     ~ flag (8 bit integer, not relevant for extension)
*     ~ Size of the parameter array (See parameter array)
*     ~ Parameter array (property array which defined which parameters
*       are expected by the function)
*     ~ Return value (property definition for the return value. This might be NULL if the function does not return anything)
*     ~ Name of the function (used to display in EB GUIDE Studio) -> THIS IS THE UNIFIED IDENTIFIER OF THE FUNCTION => the name has to be unique
*     ~ Description of the function (used to display a description tooltip within EB GUIDE Studio)
*
*   example :
*     static const ActionDescriptor actionDesc [] =
*     {
*       // here begins the first action definition
*       { 0                                     // action flags
*       , ARRAY_SIZE(doSomethingUsefulParameters)   // action size of parameter array
*       , doSomethingUsefulParameters           // action parameter array ( NULL means that the function has no parameters)
*       , NULL                                  // action return value (NULL means the function does not return a value)
*       , "doSomethingUseful"                   // action name  (function name used to display in EB GUIDE Studio)
*       , "does something useful"               // action description (description for the function)
*       }
*   };
*/


static gtf::metainformation::PropertyDescriptor GtfFocusRectColorProperties[] =
{
    gtf::metainformation::PropertyDescriptor(&typeColor     // property type ,
    , "focusedColor"                                 // property name ,
    , "Background color for the focused rectangle."  // property description ,
    , "Appearance"
    , "169,169,169,255")                             // property category ,
};


static const gtf::metainformation::WidgetFeatureDescriptor widget_feature_desc[] =
{ WIDGET_FEATURE_WA("GtfFocusRectColorFeature" // widget feature name
, "FocusRectColor" // widget feature alias (displayed in guide)
// description
, "Adds a different background color for a focused rectangle"
// category, used in EB GUIDE Studio to group the features
, "Appearance"
// version (here: 1.0)
, 0x0100
// properties
, GtfFocusRectColorProperties
// specifies where the widget feature is applicable
// this feature is allowed only on rectangle widgets with state focused
, "w:GtfRectangle&&wf:StateFocused"
// creation function (called on creation)
, gtf::dependencyresolver::InterfaceName<focusedwidgetfeature::FocusRectColorFeature>::name())
};

gtf::metainformation::ActionDescriptor const* focusedwidgetfeature::FocusedWidgetFeatureDesc::GetActions(uint32_t& count_) const
{
    count_ = 0;
    return NULL;
}

gtf::metainformation::PopupStackDescriptor const* focusedwidgetfeature::FocusedWidgetFeatureDesc::GetPopupStacks(uint32_t& count_) const
{
    count_ = 0;
    return NULL;
}

gtf::metainformation::WidgetDescriptor const* focusedwidgetfeature::FocusedWidgetFeatureDesc::GetWidgets(uint32_t& count_) const
{
    count_ = 0;
    return NULL;
}

gtf::metainformation::WidgetFeatureDescriptor const* focusedwidgetfeature::FocusedWidgetFeatureDesc::GetWidgetFeatures(uint32_t& count_) const
{
    count_ = ARRAY_SIZE(widget_feature_desc);
    return widget_feature_desc;
}

gtf::metainformation::ResourceDescriptor const* focusedwidgetfeature::FocusedWidgetFeatureDesc::GetResourceTypes(uint32_t& count_) const
{
    count_ = 0;
    return NULL;
}

/* \brief Resolves the property descriptor arrays defined within this descriptor definition file
*/
bool focusedwidgetfeature::FocusedWidgetFeatureDesc::ResolveTypes(const gtf::typesystem::TypeManager* typeManager)
{
    bool success = true;
    /*  All property definitions have to be resolved.
    *   For this all property descriptor definition arrays need to be processed by the type manager.
    *   NOTE: Resolving has to be done by startup.
    *
    *  example:
    *   RESOLVE_PROPERTIES(NAME_OF_THE_PROPERTY_ARRAY);
    *   RESOLVE_PROPERTIES(NAME_OF_THE_FEATURE_PROPERTY_ARRAY);
    *   RESOLVE_PROPERTIES(doSomethingUsefulParameters);
    */

    // add sample widget and sample widget feature properties
    RESOLVE_PROPERTIES(GtfFocusRectColorProperties);
    return success;
}
