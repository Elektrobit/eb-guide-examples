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

#ifndef GTF_LABEL_SHAPE_ADDER_H_INCLUDED
#define GTF_LABEL_SHAPE_ADDER_H_INCLUDED

#include <gtf/scdr/DynamicPropertyAdder.h>
#include <gtf/scene/TypeResolver.h>

using namespace gtf;

namespace labelshapewidgetfeature
{
    /*!
    \brief Implementation of focus features.
    */
    class SineCurveShapeAdder : public gtf::scdr::DynamicPropertyAdder
    {
    public:
        SineCurveShapeAdder(const scene::TypeResolverHandle& resolver_);

        virtual void addToWidget(properties::Container& widget_) override;
        virtual void connect(properties::Container& model_) override;
        virtual void disconnect() override;

    private:
        gtf::scene::TypeResolver::FeaturePropertyReference labelShapeFeatureReference;
    };
} // namespace labelshapewidgetfeature

GTF_DEFINE_INTERFACE_NAME(labelshapewidgetfeature::SineCurveShapeAdder);


#endif // GTF_LABEL_SHAPE_ADDER_H_INCLUDED
