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

#include "ExtendedDynamicFeatureCollection.h"
#include "SineLabelShape.h"

using namespace gtf;

namespace labelshapewidgetfeature{

    ExtendedDynamicFeatureCollection::ExtendedDynamicFeatureCollection(const gtf::scene::TypeResolverHandle& resolver_)
    : resolver(resolver_)
    {
    }

    void ExtendedDynamicFeatureCollection::addFeaturesTo(container::Array<properties::Features::Feature>& labelShapeFeatures_) const
    {
        addIfFound(labelShapeFeatures_, SineLabelShapeFeatureName);
    }

    void ExtendedDynamicFeatureCollection::addIfFound(container::Array<properties::Features::Feature>& listToAdd_, const char* name_) const
    {
        properties::Features::Feature type = resolver->resolveFeatureType(name_);

        if (type != properties::Features::Feature())
        {
            listToAdd_.append(type);
        }
    }
} // namespace labelshapewidgetfeature