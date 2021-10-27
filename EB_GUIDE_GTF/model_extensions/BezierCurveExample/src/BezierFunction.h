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
#ifndef BEZIER_FUNCTION_H
#define BEZIER_FUNCTION_H

#include <gtf/utils/smartptr/RefCountedPtr.h>
#include <gtf/utils/math/vector2.h>

#include <gtf/container/Array.h>


namespace bezierCurve
{
    typedef gtf::utils::math::vector2f Point;

    inline float lerp(float a, float b, float t)
    {
        return a * (1.0F - t) + b * t;
    }

    inline Point lerp(Point a, Point b, float t)
    {
        return a * (1.0F - t) + b * t;
    }

    inline Point* lower_bound(Point* first_,Point* last_, const Point& value_)
    {
        Point* left = first_;
        Point* right = last_;
        while (left < right)
        {
            const ptrdiff_t offset = (right - left) / 2;
            Point* const        pivot = left + offset;

            if ((*pivot).x < value_.x)
            {
                left = pivot + 1;
            }
            else
            {
                right = pivot;
            }
        }
        return left;

    }

    class BezierFunction :public gtf::utils::smartptr::RefCounterBase<gtf::utils::smartptr::counter::SingleThreaded>
    {
    public:
        BezierFunction(Point p1_, Point p2_)
            :p0(0.0F, 0.0F), p1(p1_), p2(p2_), p3(1.0F, 1.0F)
        {
            createInterpolationTable();
        }

        bool IsEqual(Point p1_, Point p2_)
        {
            return p1.x == p1_.x && p1.y == p1_.y &&
                p2.x == p2_.x && p2.y == p2_.y;
        }

        void createInterpolationTable()
        {
            // Create lookup table to speed up interpolation later on
            static constexpr const uint32_t NUM_SAMPLES = 100U;

            // plus 1 sample to include the 1.0 value
            interpolationTable.resize(NUM_SAMPLES + 1);
            for (uint32_t i = 0U; i < NUM_SAMPLES + 1; ++i)
            {
                float t = float(i) / float(NUM_SAMPLES);

                // use de Casteljau algorithm to calculate lookup table values
                Point p01 = lerp(p0, p1, t);
                Point p12 = lerp(p1, p2, t);
                Point p23 = lerp(p2, p3, t);

                Point p01_12 = lerp(p01, p12, t);
                Point p12_23 = lerp(p12, p23, t);

                Point p = lerp(p01_12, p12_23, t);

                interpolationTable[i] = p;
            }
        }

        float interpolate(float t)
        {
            if (t == 0.0F)
            {
                return interpolationTable.begin()->y;
            }
            if (t == 1.0F)
            {
                return interpolationTable.last().y;
            }

            if (interpolationTable.empty())
            {
                return 0.0F;
            }
            Point reference(t, 0.0F);
            // search line segment from lookup table

            Point* point = lower_bound(&*interpolationTable.begin(), &interpolationTable.last(), reference);

            auto predecessor = point - 1; // check not be called when array is empty

            // linear interpolation of found line segment
            return lerp(predecessor->y, point->y, (t - predecessor->x) / (point->x - predecessor->x));
        }

    private:

        Point p0;
        Point p1;
        Point p2;
        Point p3;

        gtf::container::Array<Point> interpolationTable;
    };

    typedef gtf::utils::smartptr::RefCountedPtr<BezierFunction> BezierFunctionHandle;

    template <typename NativeType, typename TValueType>
    struct Bezier
    {
        static inline NativeType exec(NativeType start, NativeType end, float interpolatedValue)
        {

            return NativeType(float(end - start) * interpolatedValue + float(start));
        }
    };

    template <>
    struct Bezier<float, gtf::properties::FloatValue>
    {
        static inline float exec(float start, float end, float interpolatedValue)
        {
            return (end - start) * interpolatedValue + start;
        }
    };

    template <>
    struct Bezier<int32_t, gtf::properties::ColorValue>
    {
        static uint32_t exec(int32_t start, int32_t end, float interpolatedValue)
        {
            uint32_t ret;
            uint8_t* pStart = reinterpret_cast<uint8_t*>(&start);
            uint8_t* pEnd = reinterpret_cast<uint8_t*>(&end);
            uint8_t* pRet = reinterpret_cast<uint8_t*>(&ret);

            pRet[0] = uint8_t(float(pEnd[0] - pStart[0])*interpolatedValue + float(pStart[0]));
            pRet[1] = uint8_t(float(pEnd[1] - pStart[1])*interpolatedValue + float(pStart[1]));
            pRet[2] = uint8_t(float(pEnd[2] - pStart[2])*interpolatedValue + float(pStart[2]));
            pRet[3] = uint8_t(float(pEnd[3] - pStart[3])*interpolatedValue + float(pStart[3]));

            return ret;
        }
    };
}

#endif //BEZIER_FUNCTION_H