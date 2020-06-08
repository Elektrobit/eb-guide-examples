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

#include "DownscalingFeatureDesc.h"
#include "DownscalingFeature.h"

static const gtf::type::TypeBase typeInt32("int32_t");
static const gtf::type::TypeBase typeString("string");

static gtf::metainformation::PropertyDescriptor DownscalingFeatureProperties[] =
{
    PROPERTYDESCRIPTOR_WITH_DEFAULTVALUE(&typeInt32, "targetHeight", "Target image height", "Appearance", "100"),
    PROPERTYDESCRIPTOR_WITH_DEFAULTVALUE(&typeInt32,"targetWidth", "Target image width", "Appearance", "100"),
    PROPERTYDESCRIPTOR_WITH_DEFAULTVALUE(&typeString,"imagePath", "Relative path to the image", "Appearance", "")
};

static const gtf::metainformation::WidgetFeatureDescriptor widget_feature_desc[] =
{ WIDGET_FEATURE_WA("DownScalingFeature" // widget feature name
, "Downscaling" // widget feature alias (displayed in guide)
// description
, "Downscales an Image widget to a target size"
// category, used in EB GUIDE Studio to group the features
, "Appearance"
// version (here: 1.0)
, 0x0100
, DownscalingFeatureProperties
, "w:GtfImage"
, gtf::dependencyresolver::InterfaceName<downscalingFeature::DownscalingFeature>::name())
};

gtf::metainformation::ActionDescriptor const* downscalingFeature::DownscalingFeatureDesc::GetActions(uint32_t& count_) const
{
    count_ = 0;
    return NULL;
}

gtf::metainformation::PopupStackDescriptor const* downscalingFeature::DownscalingFeatureDesc::GetPopupStacks(uint32_t& count_) const
{
    count_ = 0;
    return NULL;
}

gtf::metainformation::WidgetDescriptor const* downscalingFeature::DownscalingFeatureDesc::GetWidgets(uint32_t& count_) const
{
    count_ = 0;
    return NULL;
}

gtf::metainformation::WidgetFeatureDescriptor const* downscalingFeature::DownscalingFeatureDesc::GetWidgetFeatures(uint32_t& count_) const
{
    count_ = ARRAY_SIZE(widget_feature_desc);
    return widget_feature_desc;
}

gtf::metainformation::ResourceDescriptor const* downscalingFeature::DownscalingFeatureDesc::GetResourceTypes(uint32_t& count_) const
{
    count_ = 0;
    return NULL;
}

bool downscalingFeature::DownscalingFeatureDesc::ResolveTypes(const gtf::typesystem::TypeManager* typeManager)
{
    bool success = true;

    RESOLVE_PROPERTIES(DownscalingFeatureProperties);

    return success;
}
