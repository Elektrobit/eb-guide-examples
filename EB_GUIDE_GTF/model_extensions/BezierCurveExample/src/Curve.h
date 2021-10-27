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

#ifndef GTF_ANIMATION_CURVE_CURVE_H_INCLUDED
#define GTF_ANIMATION_CURVE_CURVE_H_INCLUDED

#include "DelayedFixedCurve.h"
#include "Properties.h"

namespace bezierCurve
{
    template <typename NativeType, typename TValueType>
    struct LinearAddition
    {
        static inline NativeType exec(NativeType v1, NativeType v2)
        {
            return v1 + v2;
        }
    };

    template<typename NativeType, typename TValueType, const char* name>
    class Curve : public DelayedFixedCurve
    {
    public:
        Curve(const gtf::scene::TypeResolverHandle& resolver)
            : DelayedFixedCurve(resolver), initialValue(false)
        {
            targetPropertyReference.containerType.name = name;
            targetPropertyReference.property.name = propertyname::target;

            resolver->resolveContainerProperties(&targetPropertyReference, 1);
        }

        // Inherited via AnimationBase
        virtual void connect(gtf::properties::Container & model_)
        {
            DelayedFixedCurve::connect(model_);
            gtf::properties::Properties& widgetProperties = model_.getProperties();
            targetValue = widgetProperties.get<TValueType>(targetPropertyReference.property.identifier);
        }

        virtual void disconnect()
        {
            targetValue.reset();
            DelayedFixedCurve::disconnect();
        }

    protected:
        virtual void executeCurve(
            int32_t
            , uint32_t curveTime
            , uint32_t duration
            , bool relative
        )
        {
            GTF_UNUSED_PARAM(duration);
            NativeType newValue = 0;
            if (relative)
            {
                bool canGetinitialValue = getInitialValue(newValue);

                if (!canGetinitialValue)
                {
                    return;
                }
            }
            newValue = LinearAddition<NativeType, TValueType>::exec(
                newValue,
                executeFunction(curveTime, duration)
            );

            targetValue->set(newValue);
        }

        bool getInitialValue(NativeType& value)
        {
            if (!initialValue)
            {
                value = targetValue->get();
                initialValue = true;
            }
            return initialValue;
        }

        void clearInitialValue()
        {
            initialValue = false;
        }

        virtual NativeType executeFunction(uint32_t, uint32_t)
        {
            NativeType ret = 0;
            return ret;
        }

        bool initialValue;

        gtf::scene::TypeResolver::ContainerPropertyReference targetPropertyReference;

        gtf::utils::smartptr::RefCountedPtr<TValueType> targetValue;
    };
}
#endif // GTF_ANIMATION_CURVE_CURVE_H_INCLUDED
