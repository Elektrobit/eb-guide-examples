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

#include <gtf/metainformation/DescriptorProvider.h>
#include <gtf/metainformation/WidgetDescriptor.h>
#include <gtf/typesystem/TypeManager.h>

#include "PreparedStageImpl.h"

// pre-defined property types
static const gtf::type::TypeBase s_typeInt32("int32_t");               // int32_t property
static const gtf::type::TypeBase s_typeFloat("float");                 // float property
static const gtf::type::TypeTypedef s_typeColor("color", s_typeInt32); // color property. Base type is int32_t

static gtf::metainformation::PropertyDescriptor CrepuscularEffectWidgetProperties[7] =
{
    gtf::metainformation::PropertyDescriptor(&s_typeColor
    , "color"
    , "The effect color"
    , "Common"
    , "0, 248, 0, 255"),

    gtf::metainformation::PropertyDescriptor(&s_typeFloat// property type
    , "x-pos"                      // property name
    , "The x origin of the effect" // property description
    , "Common"                     // property category
    , "0"),                        // default value

    gtf::metainformation::PropertyDescriptor(&s_typeFloat// property type
    , "y-pos"                      // property name
    , "The y origin of the effect" // property description
    , "Common"                     // property category
    , "0"),                        // default value

    gtf::metainformation::PropertyDescriptor(&s_typeInt32
    , "samples"
    , "The number of samples"
    , "Common"
    , "16"),

    gtf::metainformation::PropertyDescriptor(&s_typeFloat
    , "decay"
    , "The decay parameter"
    , "Common"
    , "1.0"),

    gtf::metainformation::PropertyDescriptor(&s_typeFloat
    , "density"
    , "The density parameter"
    , "Common"
    , "1.0"),

    gtf::metainformation::PropertyDescriptor(&s_typeFloat
    , "weight"
    , "The weight parameter"
    , "Common"
    , "1.0")
};

struct CWidgetDescriptor : public gtf::metainformation::WidgetDescriptor
{
    CWidgetDescriptor(
    const char*               name_,
    const char*               alias_,
    const char*               description_,
    const gtf::metainformation::PropertyDescriptor* properties_,
    uint16_t                  propertyCount_,
    const char*           interfaceName_)
    {
        this->m_pName            = name_;
        this->m_pAlias           = alias_;
        this->m_pDescription     = description_;
        this->m_pCategory        = "Custom widgets";
        this->m_pBaseClassName   = "GtfAbstractVisualWidget";
        this->m_fIsAbstract      = false;
        this->m_version          = 0x0100;
        this->m_fIsView          = false;
        this->m_fChildrenAllowed = true;
        this->m_fIsInstantiator  = false;
        this->m_fIsEffect        = false;
        this->m_numProperties    = propertyCount_;
        this->m_pProperties      = properties_;
        this->interfaceName    = interfaceName_;

    }
};

class CrepuscularDescriptorProvider : public gtf::metainformation::DescriptorProvider
{
public:
    virtual gtf::metainformation::ActionDescriptor const* GetActions(uint32_t &count) const               { count = 0; return NULL; }
    virtual gtf::metainformation::PopupStackDescriptor const* GetPopupStacks(uint32_t &count) const       { count = 0; return NULL; }
    virtual gtf::metainformation::WidgetDescriptor const* GetWidgets(uint32_t &count) const               { count = 1; return &widgetDescriptor; }
    virtual gtf::metainformation::WidgetFeatureDescriptor const* GetWidgetFeatures(uint32_t &count) const { count = 0; return NULL; }
    virtual gtf::metainformation::ResourceDescriptor const* GetResourceTypes(uint32_t &count) const       { count = 0; return NULL; }
    virtual bool ResolveTypes(const gtf::typesystem::TypeManager* typeManager_)
    {
        bool success = true;
        for (uint32_t propertyTypeIndex = 0; propertyTypeIndex < ARRAY_SIZE(CrepuscularEffectWidgetProperties); ++propertyTypeIndex)
        {
            gtf::metainformation::PropertyDescriptor *curProperty = &(CrepuscularEffectWidgetProperties[propertyTypeIndex]);
            curProperty->m_pType            = typeManager_->getTypeByType(curProperty->m_pUnresolvedType);
            success                         = success && curProperty->m_pType;
        }
        return success;
    }

    static const CWidgetDescriptor widgetDescriptor;
};

const CWidgetDescriptor CrepuscularDescriptorProvider::widgetDescriptor(
    CREPUSCULAR_PLUGIN_NAME,
    "Crepuscular",
    "Fancy effect",
    CrepuscularEffectWidgetProperties,
    ARRAY_SIZE(CrepuscularEffectWidgetProperties),
    NULL);

crepuscular::PreparedStageImpl::PreparedStageImpl(const gtf::dependencyresolver::DependencyContainerHandle& container_)
    : container(container_)
{
    descriptors = new CrepuscularDescriptorProvider();

    container->registerInstance<gtf::metainformation::DescriptorProvider>(descriptors);
}

crepuscular::PreparedStageImpl::~PreparedStageImpl()
{
    container->unregisterInstance(descriptors);
}
