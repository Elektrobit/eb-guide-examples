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

#include "ScriptFunctionDescriptorProvider.h"
#include "ScriptFunctionNames.h"

// Include for the type manager. The type manager knows which properties types are available and can resolve a type for a property.
#include <gtf/typesystem/TypeManager.h>
#include <gtf/types/Macros.h> // include helper macros

// Includes for the descriptor interfaces. The descriptor interfaces define what we can add to the GTF Runtime.
#include <gtf/metainformation/PropertyDescriptor.h> // for property definitions which are used by the following :
#include <gtf/metainformation/ActionDescriptor.h>   // *         Action Descriptions

// These are property definitions which can be used in the widget, widget feature and action definitions.
static gtf::type::TypeBase const typeInt32("int32_t");

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
*   static gtf::metainformation::PropertyDescriptor NAME_OF_THE_PROPERTY_ARRAY[] =
*   {
*     gtf::metainformation::PropertyDescriptor(
*       &s_typeString                               // property type
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

static gtf::metainformation::PropertyDescriptor parameters[] =
{ gtf::metainformation::PropertyDescriptor(&typeInt32 // property type
                                    ,
                                    "Operand1" // property name
                                    ,
                                    "First Operand" // property description
                                    ),
  gtf::metainformation::PropertyDescriptor(&typeInt32 // property type
                                    ,
                                    "Operand2" // property name
                                    ,
                                    "Second Operand" // property description
                                    ) };

static gtf::metainformation::PropertyDescriptor returnValue[] =
{ gtf::metainformation::PropertyDescriptor(&typeInt32, "Returned value", "Result of the operation") };

static const gtf::metainformation::ActionDescriptor actionDesc[] =
{ { ARRAY_SIZE(parameters), parameters, returnValue, SUMFUNCTION, "Returns the sum of two integer values." } };

/* \brief returns the action descriptor array
*/
gtf::metainformation::ActionDescriptor const* ScriptFunctionDescriptorProvider::GetActions(uint32_t& count) const
{
    count = ARRAY_SIZE(actionDesc);
    return actionDesc;
}

/* \brief resolves the property descriptor arrays defined within this descriptor definition file
*/
bool ScriptFunctionDescriptorProvider::ResolveTypes(const gtf::typesystem::TypeManager* typeManager)
{
    /*  All property definitions have to be resolved.
    *   For this all property descriptor definition arrays need to be processed by the type manager.
    *   NOTE: Resolving has to be done by startup.
    */
    bool success = true;

    RESOLVE_PROPERTIES(parameters);
    RESOLVE_PROPERTIES(returnValue);

    return success;
}
