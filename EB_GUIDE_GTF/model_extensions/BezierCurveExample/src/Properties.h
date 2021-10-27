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

#ifndef BEZIER_CURVE_PROPERTIES_H
#define BEZIER_CURVE_PROPERTIES_H
namespace bezierCurve
{
    namespace propertyname
    {
        extern const char DelayedFixedCurve[];
        extern const char BezierCurveBool[];
        extern const char BezierCurveS32[];
        extern const char BezierCurveColor[];
        extern const char BezierCurveFloat[];

        extern const char handle1x[];
        extern const char handle1y[];
        extern const char handle2x[];
        extern const char handle2y[];

        extern const char start[];
        extern const char end[];
        extern const char target[];

        extern const char enabled[];
        extern const char delay[];
        extern const char duration[];
        extern const char repeat[];
        extern const char alternating[];
        extern const char relative[];
    }
}

#endif // BEZIER_CURVE_PROPERTIES_H
