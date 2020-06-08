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

#ifndef GTF_LABEL_SHAPE_FEATURE_ADDER_H_INCLUDED
#define GTF_LABEL_SHAPE_FEATURE_ADDER_H_INCLUDED

#include <gtf/scdr/DynamicFeatureCollection.h>
#include <gtf/scene/TypeResolver.h>

using namespace gtf;

namespace labelshapewidgetfeature {
    /*!
    \brief Implementation of label shape features adder for label shape.
    */
    class ExtendedDynamicFeatureCollection : public scdr::DynamicFeatureCollection
    {
    public:
        ExtendedDynamicFeatureCollection(const scene::TypeResolverHandle& resolver_);
        virtual void addFeaturesTo(container::Array<properties::Features::Feature>& labelShapeFeatures_) const override;
        void addIfFound(container::Array<properties::Features::Feature>& listToAdd_, const char* name_) const;

    private:
        scene::TypeResolverHandle resolver;
    };
} // namespace labelshapewidgetfeature

#endif // GTF_LABEL_SHAPE_FEATURE_ADDER_H_INCLUDED