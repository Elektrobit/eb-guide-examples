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

#ifndef BEZIER_CURVE_DELAYED_FIXED_CURVE_H
#define BEZIER_CURVE_DELAYED_FIXED_CURVE_H

#include <gtf/scene/TypeResolver.h>
#include <gtf/properties/Container.h>
#include <gtf/properties/ScalarValue.h>
#include <gtf/coreruntime/CoreContext.h>
#include <gtf/types/Noncopyable.h>

#include <gtf/animation/AnimationBase.h>

namespace bezierCurve
{
    class DelayedFixedCurve : public gtf::animation::AnimationBase, public gtf::types::Noncopyable
    {
    public:
        DelayedFixedCurve(const gtf::scene::TypeResolverHandle& resolver);

        uint32_t getRepeat();

        bool isBeyond(int32_t time) const;
        bool isBefore(int32_t time) const;
        uint32_t getExecutionTime();
        bool isInfinite();

        void reset(bool freshStart, uint32_t resetTime);
        AnimationBase::ExecStatus execute(int32_t diff, int32_t animationTime, bool forwards);

        // Inherited via AnimationBase
        virtual void connect(gtf::properties::Container & model);
        virtual void disconnect();

    protected:
        gtf::properties::Container* model;
        int32_t lastExecutionTime, currentExecutionTime;

        /*!
            * \brief Execute curve formula
            *
            * The given time \a curveTime is the time in milliseconds within the curve segment.
            * So delay, alternating and repeat counts have already been taken into account.
            *
            * \param diff holds the time differene between the last execution and the current execution - on the first call it will be zero
            * \param relative is true when the curve shall add the value to the target property - which is read when the curve starts.
            * \param curveTime time within curve segments in milliseconds.
            */
        virtual void executeCurve(
            int32_t diff
            , uint32_t curveTime
            , uint32_t duration
            , bool relative
        ) = 0;

        virtual void clearInitialValue() = 0;
        int32_t calculateNewCurveTime(int32_t time);
        int32_t calculateAlternatingCurveTime(int32_t animationTime, int32_t delay, int32_t duration, int32_t endTime, int32_t repeat, bool relative);
        int32_t calculateRepeatedCurveTime(int32_t animationTime, int32_t delay, int32_t duration, int32_t endTime, int32_t repeat, bool relative);

        struct ContainerProperties
        {
            enum
            {
                enabled,
                delay,
                duration,
                repeat,
                alternating,
                relative,
                Count
            };
        };
        gtf::scene::TypeResolver::ContainerPropertyReference containerReferences[ContainerProperties::Count];

        gtf::properties::BoolValueHandle     enabledValue;
        gtf::properties::IntegerValueHandle  delayValue;
        gtf::properties::IntegerValueHandle  durationValue;
        gtf::properties::IntegerValueHandle  repeatValue;
        gtf::properties::BoolValueHandle     alternatingValue;
        gtf::properties::BoolValueHandle     relativeValue;
    };
}



#endif // BEZIER_CURVE_DELAYED_FIXED_CURVE_H
