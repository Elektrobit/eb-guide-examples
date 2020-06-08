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

#include <gtf/coreruntime/CoreContext.h>
#include "DelayedFixedCurve.h"
#include "Properties.h"

#include "Tracing.h"


namespace bezierCurve {
    DelayedFixedCurve::DelayedFixedCurve(const gtf::scene::TypeResolverHandle& resolver)
        : model(NULL)
        , lastExecutionTime(0)
        , currentExecutionTime(0)
    {
        containerReferences[ContainerProperties::enabled].containerType.name = propertyname::DelayedFixedCurve;
        containerReferences[ContainerProperties::enabled].property.name = propertyname::enabled;
        containerReferences[ContainerProperties::alternating].containerType.name = propertyname::DelayedFixedCurve;
        containerReferences[ContainerProperties::alternating].property.name = propertyname::alternating;
        containerReferences[ContainerProperties::repeat].containerType.name = propertyname::DelayedFixedCurve;
        containerReferences[ContainerProperties::repeat].property.name = propertyname::repeat;
        containerReferences[ContainerProperties::delay].containerType.name = propertyname::DelayedFixedCurve;
        containerReferences[ContainerProperties::delay].property.name = propertyname::delay;
        containerReferences[ContainerProperties::duration].containerType.name = propertyname::DelayedFixedCurve;
        containerReferences[ContainerProperties::duration].property.name = propertyname::duration;
        containerReferences[ContainerProperties::relative].containerType.name = propertyname::DelayedFixedCurve;
        containerReferences[ContainerProperties::relative].property.name = propertyname::relative;

        resolver->resolveContainerProperties(containerReferences, ContainerProperties::Count);
    }

    uint32_t DelayedFixedCurve::getRepeat()
    {
        return repeatValue->get();
    }

    void DelayedFixedCurve::reset(bool freshStart, uint32_t resetTime)
    {
        currentExecutionTime = lastExecutionTime = int32_t(resetTime);

        if (freshStart)
        {
            clearInitialValue();
        }
    }

    int32_t DelayedFixedCurve::calculateAlternatingCurveTime(int32_t curveTime, int32_t delay, int32_t duration, int32_t endTime, int32_t repeat, bool relative)
    {
        int32_t previousRepeat = (lastExecutionTime - delay) / duration;
        int32_t currentRepeat = curveTime / duration;
        // alternating means we have to mimic a negative time vector
        // everytime we pass the duration time step in one direction
        bool isReverse = ((curveTime / duration) & 1) != 0;
        if (!isReverse)
        {
            curveTime = curveTime % duration;

            // handle final step
            if (repeat > 1 && isBeyond(endTime))
            {
                curveTime = 0;
            }
            else if (relative && (previousRepeat != currentRepeat))
            { // when the border is crossed the curve needs reevaluation to
                // ensure that the base value for the next run is met.
                GTF_DEBUG(BezierCurveExample, "DelayedFixedCurve::executing Curve " << this->model << " with t:0 diff:0 duration:" << duration << " to reset");
                executeCurve(0, 0, duration, relative);
            }
        }
        else
        {
            curveTime = duration - (curveTime % duration);

            // handle final step
            if (repeat > 1 && isBeyond(endTime))
            {
                curveTime = duration;
            }
        }

        return curveTime;
    }

    int32_t DelayedFixedCurve::calculateRepeatedCurveTime(int32_t curveTime, int32_t delay, int32_t duration, int32_t endTime, int32_t repeat, bool relative)
    {
        int32_t previousRepeat = (lastExecutionTime - delay) / duration;
        int32_t currentRepeat = curveTime / duration;
        // last repeated round .. then execute the final state:
        if (repeat > 1 && isBeyond(endTime))
        {
            curveTime = duration;
        }
        else if (repeat > 1 && isBefore(0))
        {
            curveTime = 0;
        }
        else
        {
            // not alternating .. means we have to clamp the time using modulo into
            // the interval
            int32_t clampedTime = curveTime % duration;
            curveTime = (curveTime != 0 && clampedTime == 0) ? duration : clampedTime;

            if (relative && (previousRepeat != currentRepeat))
            {
                // when the border is crossed the curve needs reevaluation to
                // ensure that the base value for the next run is met.
                GTF_DEBUG(BezierCurveExample, "DelayedFixedCurve::executing Curve " << this->model << " with t:" << duration << " diff:0 duration:" << duration << " to reset");
                executeCurve(0, duration, duration, relative);
            }
        }

        return curveTime;
    }

    int32_t DelayedFixedCurve::calculateNewCurveTime(int32_t animationTime)
    {
        int32_t duration = durationValue->get();
        int32_t delay = delayValue->get();
        int32_t repeat = repeatValue->get();

        int32_t endTime = delay + duration * repeat;
        if (repeat == 0)
        {
            endTime = delay + duration;
        }
        bool                 alternating = alternatingValue->get();
        bool                 relative = relativeValue->get();
        auto curveTime = animationTime - delay;

        if (curveTime > 0)
        {
            if (repeat != 1)
            {
                if ((repeat == 0) || (animationTime <= endTime) || isBeyond(endTime))
                {
                    if (!alternating)
                    {
                        curveTime = calculateRepeatedCurveTime(curveTime, delay, duration, endTime, repeat, relative);
                    }
                    else
                    {
                        curveTime = calculateAlternatingCurveTime(curveTime, delay, duration, endTime, repeat, relative);
                    }
                }
            }
            else if (isBeyond(endTime)) // single execution
            {
                curveTime = duration;
            }
        }
        else if (isBefore(delay))
        {
            curveTime = 0;
        }

        if ((curveTime > duration) || (curveTime < 0))
        {
            return 0;
        }

        return curveTime;
    }

    gtf::animation::AnimationBase::ExecStatus
        DelayedFixedCurve::execute(int32_t diff, int32_t animationTime, bool curveForwards)
    {
        if (!model)
        {
            return AnimationComplete;
        }

        currentExecutionTime = animationTime;
        GTF_DEBUG(BezierCurveExample, "DelayedFixedCurve::execute " << this->model << " from " << lastExecutionTime << " to " << animationTime);

        if (!enabledValue->get())
        {
            return AnimationComplete;
        }

        int32_t duration = durationValue->get();
        int32_t delay = delayValue->get();

        if (duration == 0)
        {
            GTF_WARNING(BezierCurveExample, "[DelayedFixedCurve::execute] duration is zero - configuration error");
            return AnimationComplete;
        }

        int32_t curveTime = calculateNewCurveTime(animationTime);
        if (curveTime > 0)
        {
            bool relative = relativeValue->get();

            executeCurve(diff, curveTime, duration, relative);
        }
        else
        {
            GTF_DEBUG(BezierCurveExample, "DelayedFixedCurve::executing not executing " << this->model << "before delay " << animationTime);
        }

        int32_t repeat = repeatValue->get();

        ExecStatus ret = AnimationContinue;

        if (repeat != 0)
        {
            if (curveForwards)
            {
                int32_t endTime = delay + repeat * duration;
                if (currentExecutionTime >= endTime)
                {
                    ret = AnimationComplete;
                }
            }
            else
            {
                if (animationTime <= 0)
                {
                    ret = AnimationComplete;
                }
            }
        }

        lastExecutionTime = currentExecutionTime;
        GTF_DEBUG(BezierCurveExample, this->model << " completed with : " << ret);
        return ret;
    }

    void DelayedFixedCurve::connect(gtf::properties::Container& model_)
    {
        model = &model_;

        gtf::properties::Properties& widgetProperties = model_.getProperties();
        enabledValue = widgetProperties.get<gtf::properties::BoolValue>(containerReferences[ContainerProperties::enabled].property.identifier);
        delayValue = widgetProperties.get<gtf::properties::IntegerValue>(containerReferences[ContainerProperties::delay].property.identifier);
        durationValue = widgetProperties.get<gtf::properties::IntegerValue>(containerReferences[ContainerProperties::duration].property.identifier);
        repeatValue = widgetProperties.get<gtf::properties::IntegerValue>(containerReferences[ContainerProperties::repeat].property.identifier);
        alternatingValue = widgetProperties.get<gtf::properties::BoolValue>(containerReferences[ContainerProperties::alternating].property.identifier);
        relativeValue = widgetProperties.get<gtf::properties::BoolValue>(containerReferences[ContainerProperties::relative].property.identifier);
    }

    void DelayedFixedCurve::disconnect()
    {
        enabledValue.reset();
        delayValue.reset();
        durationValue.reset();
        repeatValue.reset();
        alternatingValue.reset();
        relativeValue.reset();

        model = NULL;
    }

    bool DelayedFixedCurve::isBeyond(int32_t t) const
    {
        GTF_DEBUG(BezierCurveExample, "isBeyond: " << ((lastExecutionTime < t && t <= currentExecutionTime) ? "true" : "false") << " : " << lastExecutionTime << " < " << t << " <= " << currentExecutionTime);
        return (lastExecutionTime < t) && (t <= currentExecutionTime);
    }

    bool DelayedFixedCurve::isBefore(int32_t t) const
    {
        GTF_DEBUG(BezierCurveExample, "isBefore: " << ((lastExecutionTime > t && t >= currentExecutionTime) ? "true" : "false") << " : " << lastExecutionTime << " > " << t << " >= " << currentExecutionTime);
        return (lastExecutionTime > t) && (t >= currentExecutionTime);
    }

    uint32_t DelayedFixedCurve::getExecutionTime()
    {
        if (!enabledValue->get())
        {
            return 0;
        }
        int32_t repeatCount = repeatValue->get();
        if (repeatCount == 0)
        {
            repeatCount = 1;
        }
        return uint32_t(delayValue->get() + repeatCount * durationValue->get());
    }

    bool DelayedFixedCurve::isInfinite()
    {
        if (!enabledValue->get())
        {
            return false;
        }
        return 0 == repeatValue->get();
    }

}

