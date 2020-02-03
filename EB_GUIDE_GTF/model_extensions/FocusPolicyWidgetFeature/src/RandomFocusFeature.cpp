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

#include "RandomFocusFeature.h"

namespace focusedpolicywidgetfeature {
RandomFocusFeature::RandomFocusFeature(const gtf::scene::TypeResolverHandle& resolver_)
    : resolver(resolver_)
{
}

void RandomFocusFeature::getFeatures(gtf::container::Array<gtf::properties::Features::Feature>& focusFeatures)
{
    addIfFound(focusFeatures, "RandomFocusPolicy");
}

void RandomFocusFeature::addIfFound(gtf::container::Array<gtf::properties::Features::Feature>& listToAdd, const char* name)
{
    gtf::properties::Features::Feature type = resolver->resolveFeatureType(name);

    if (type != gtf::properties::Features::Feature())
    {
        listToAdd.append(type);
    }
}
} // namespace focusedpolicywidgetfeature
