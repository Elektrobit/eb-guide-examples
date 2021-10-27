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

#ifndef BEZIER_CURVE_SCRIPT_CURVE_H_INCLUDED
#define BEZIER_CURVE_SCRIPT_CURVE_H_INCLUDED

#include "Curve.h"
#include "Properties.h"
#include "BezierFunction.h"

#include <gtf/datapool/ScalarValue.h>
#include <gtf/properties/Function.h>

namespace bezierCurve {

    template <typename NativeType, typename TValueType, uint32_t StackType, const char* name>
    class BezierCurve : public Curve<NativeType, TValueType, name>
    {
    public:
        BezierCurve(const gtf::scene::TypeResolverHandle& resolver)
            : Curve<NativeType, TValueType, name>(resolver)
        {
            curvePropertyReference[ContainerProperties::handle1x].containerType.name = name;
            curvePropertyReference[ContainerProperties::handle1x].property.name = propertyname::handle1x;

            curvePropertyReference[ContainerProperties::handle1y].containerType.name = name;
            curvePropertyReference[ContainerProperties::handle1y].property.name = propertyname::handle1y;

            curvePropertyReference[ContainerProperties::handle2x].containerType.name = name;
            curvePropertyReference[ContainerProperties::handle2x].property.name = propertyname::handle2x;

            curvePropertyReference[ContainerProperties::handle2y].containerType.name = name;
            curvePropertyReference[ContainerProperties::handle2y].property.name = propertyname::handle2y;

            curvePropertyReference[ContainerProperties::start].containerType.name = name;
            curvePropertyReference[ContainerProperties::start].property.name = propertyname::start;

            curvePropertyReference[ContainerProperties::end].containerType.name = name;
            curvePropertyReference[ContainerProperties::end].property.name = propertyname::end;

            resolver->resolveContainerProperties(curvePropertyReference, ContainerProperties::Count);
        }

        // Inherited via AnimationBase
        virtual void connect(gtf::properties::Container& model_)
        {
            Curve<NativeType, TValueType, name>::connect(model_);
            gtf::properties::Properties& widgetProperties = model_.getProperties();

            handle1xValue = widgetProperties.get<gtf::properties::FloatValue>(curvePropertyReference[ContainerProperties::handle1x].property.identifier);
            handle1yValue = widgetProperties.get<gtf::properties::FloatValue>(curvePropertyReference[ContainerProperties::handle1y].property.identifier);
            handle2xValue = widgetProperties.get<gtf::properties::FloatValue>(curvePropertyReference[ContainerProperties::handle2x].property.identifier);
            handle2yValue = widgetProperties.get<gtf::properties::FloatValue>(curvePropertyReference[ContainerProperties::handle2y].property.identifier);
            startValue = widgetProperties.get<TValueType>(curvePropertyReference[ContainerProperties::start].property.identifier);
            endValue = widgetProperties.get<TValueType>(curvePropertyReference[ContainerProperties::end].property.identifier);

            Point p1(handle1xValue->get(), handle1yValue->get());
            Point p2(handle2xValue->get(), handle2yValue->get());

            function = new BezierFunction(p1, p2);
        }

        virtual void disconnect()
        {
            handle1xValue.reset();
            handle1yValue.reset();
            handle2xValue.reset();
            handle2yValue.reset();
            function.reset();
            startValue.reset();
            endValue.reset();

            Curve<NativeType, TValueType, name>::disconnect();
        }

    protected:
        virtual NativeType executeFunction(uint32_t curveTime, uint32_t duration)
        {
            float scale = float(curveTime) / float(duration);

            NativeType returnVal = Bezier<NativeType, TValueType>::exec(startValue->get(), endValue->get(), function->interpolate(scale));

            return returnVal;
        }

        struct ContainerProperties
        {
            enum
            {
                handle1x,
                handle1y,
                handle2x,
                handle2y,
                start,
                end,
                Count
            };
        };
        gtf::scene::TypeResolver::ContainerPropertyReference curvePropertyReference[ContainerProperties::Count];


        gtf::properties::FloatValueHandle handle1xValue;
        gtf::properties::FloatValueHandle handle1yValue;
        gtf::properties::FloatValueHandle handle2xValue;
        gtf::properties::FloatValueHandle handle2yValue;
        gtf::utils::smartptr::RefCountedPtr<TValueType> startValue;
        gtf::utils::smartptr::RefCountedPtr<TValueType> endValue;
        BezierFunctionHandle function;
    };


    typedef BezierCurve<int32_t, gtf::properties::IntegerValue, gtf::type::TypeId::eTypeId_int32, propertyname::BezierCurveS32> BezierCurveInteger;
    typedef BezierCurve<int32_t, gtf::properties::ColorValue, gtf::type::TypeId::eTypeId_color, propertyname::BezierCurveColor> BezierCurveColor;
    typedef BezierCurve<float, gtf::properties::FloatValue, gtf::type::TypeId::eTypeId_float, propertyname::BezierCurveFloat>   BezierCurveFloat;
}

GTF_DEFINE_INTERFACE_NAME(bezierCurve::BezierCurveInteger)
GTF_DEFINE_INTERFACE_NAME(bezierCurve::BezierCurveColor)
GTF_DEFINE_INTERFACE_NAME(bezierCurve::BezierCurveFloat)

#endif // BEZIER_CURVE_SCRIPT_CURVE_H_INCLUDED
