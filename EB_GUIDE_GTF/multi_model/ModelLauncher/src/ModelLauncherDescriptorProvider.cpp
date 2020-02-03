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

#include "ModelLauncherDescriptorProvider.h"
#include "ModelLauncherNames.h"

// Include for the type manager. The type manager knows which properties types are available and can resolve a type for a property.
#include <gtf/typesystem/TypeManager.h>
#include <gtf/types/Macros.h> // include helper macros

// Includes for the descriptor interfaces. The descriptor interfaces define what we can add to the GTF Runtime.
#include <gtf/metainformation/PropertyDescriptor.h> // for property definitions which are used by the following :
#include <gtf/metainformation/ActionDescriptor.h>   // *         Action Descriptions

// These are property definitions which can be used in the widget, widget feature and action definitions.
static const gtf::type::TypeBase typeInt32("int32_t");               // int32_t property
static const gtf::type::TypeBase typeString("string");                 // string property

static gtf::metainformation::PropertyDescriptor startParameters[] =
    {
        gtf::metainformation::PropertyDescriptor(&typeString // property type
            , "ModelPath" // property name
            , "Path to the model" // property description
            )
    };

static gtf::metainformation::PropertyDescriptor stopParameters[] =
    {
        gtf::metainformation::PropertyDescriptor(&typeInt32 // property type
            , "ModelIdentifier" // property name
            , "Identifier of the model" // property description
            )
    };

static gtf::metainformation::PropertyDescriptor startReturnValue[] =
{ gtf::metainformation::PropertyDescriptor(&typeInt32, "Returned value", "Model identifier") };

static const gtf::metainformation::ActionDescriptor actionDesc[] =
    { 
        { ARRAY_SIZE(startParameters), startParameters, startReturnValue, STARTMODELFUNCTION, "Starts a model with a given path relative to the location <model_requesting>/../<model_to_start>. This method returns the identifier of the model as UUID." },
        { ARRAY_SIZE(stopParameters), stopParameters, NULL, STOPMODELFUNCTION, "Stops a model with a given identifier of the model as UUID." } 
    };

/* \brief returns the action descriptor array
*/
gtf::metainformation::ActionDescriptor const* ModelLauncherDescriptorProvider::GetActions(uint32_t& count) const
{
    count = ARRAY_SIZE(actionDesc);
    return actionDesc;
}

/* \brief resolves the property descriptor arrays defined within this descriptor definition file
*/
bool ModelLauncherDescriptorProvider::ResolveTypes(const gtf::typesystem::TypeManager* typeManager)
{
    /*  All property definitions have to be resolved.
    *   For this all property descriptor definition arrays need to be processed by the type manager.
    *   NOTE: Resolving has to be done by startup.
    */
    bool success = true;

    RESOLVE_PROPERTIES(startParameters);
    RESOLVE_PROPERTIES(startReturnValue);
    RESOLVE_PROPERTIES(stopParameters);

    return success;
}
