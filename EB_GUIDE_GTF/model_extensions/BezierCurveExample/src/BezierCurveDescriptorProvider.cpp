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


#include <gtf/typesystem/TypeManager.h>
#include <gtf/metainformation/WidgetDescriptorMacros.h>
#include "BezierCurveDescriptorProvider.h"
#include "Properties.h"
#include "BezierCurve.h"

static const char s_Empty[] = "";
static const char s_target[] = "Target";
static const char s_curveSettings[] = "CurveSettings";
static const char s_Animation[] = "Animation";

static const gtf::type::TypeBase    s_typeFloat("float");
static const gtf::type::TypeBase    s_typeInt32("int32_t");
static const gtf::type::TypeTypedef s_typeColor("color", s_typeInt32);

#define BEZIER_CURVE(NAME, TYPE, ONE, DEFAULT) static gtf::metainformation::PropertyDescriptor NAME[] =                                               \
                                                           {                                                                                                  \
                                                               PROPERTYDESCRIPTOR_WITH_DEFAULTVALUE(&s_typeFloat, bezierCurve::propertyname::handle1x, "x coordinate of the first handle", s_curveSettings, "0.0"), \
                                                               PROPERTYDESCRIPTOR_WITH_DEFAULTVALUE(&s_typeFloat, bezierCurve::propertyname::handle1y, "y coordinate of the first handle", s_curveSettings, "0.0"), \
                                                               PROPERTYDESCRIPTOR_WITH_DEFAULTVALUE(&s_typeFloat, bezierCurve::propertyname::handle2x, "x coordinate of the second handle", s_curveSettings, "0.0"), \
                                                               PROPERTYDESCRIPTOR_WITH_DEFAULTVALUE(&s_typeFloat, bezierCurve::propertyname::handle2y, "y coordinate of the second handle", s_curveSettings, "0.0"), \
                                                               PROPERTYDESCRIPTOR_WITH_DEFAULTVALUE(TYPE, bezierCurve::propertyname::start, "Initial position", s_curveSettings, DEFAULT), \
                                                               PROPERTYDESCRIPTOR_WITH_DEFAULTVALUE(TYPE, bezierCurve::propertyname::end, "End position", s_curveSettings, ONE),       \
                                                               PROPERTYDESCRIPTOR_WITH_DEFAULTVALUE(TYPE, bezierCurve::propertyname::target, "Curve output value", s_target, DEFAULT),       \
                                                           }


BEZIER_CURVE(BezierCurve32Properties, &s_typeInt32, "1", "0");
BEZIER_CURVE(BezierCurveColorProperties, &s_typeColor, "10,10,10,10", "0,0,0,255");
BEZIER_CURVE(BezierCurveFloatProperties, &s_typeFloat, "1.0", "0.0");

static const gtf::metainformation::WidgetDescriptor widget_desc[] =
{
    PRIMITIVE_WIDGET_WO_CHILDREN(bezierCurve::propertyname::BezierCurveS32,"Bezier curve" ,s_Empty, s_Animation,   bezierCurve::propertyname::DelayedFixedCurve, false, 0x0100, BezierCurve32Properties, gtf::dependencyresolver::InterfaceName<bezierCurve::BezierCurveInteger>::name()),
    PRIMITIVE_WIDGET_WO_CHILDREN(bezierCurve::propertyname::BezierCurveColor,"Bezier curve" ,s_Empty, s_Animation, bezierCurve::propertyname::DelayedFixedCurve, false, 0x0100, BezierCurveColorProperties, gtf::dependencyresolver::InterfaceName<bezierCurve::BezierCurveColor>::name()),
    PRIMITIVE_WIDGET_WO_CHILDREN(bezierCurve::propertyname::BezierCurveFloat,"Bezier curve" ,s_Empty, s_Animation, bezierCurve::propertyname::DelayedFixedCurve, false, 0x0100, BezierCurveFloatProperties, gtf::dependencyresolver::InterfaceName<bezierCurve::BezierCurveFloat>::name()),
};

gtf::metainformation::WidgetDescriptor const* bezierCurve::BezierCurveDescriptorProvider::GetWidgets(uint32_t& count_) const
{
    count_ = ARRAY_SIZE(widget_desc);
    return widget_desc;
}

bool bezierCurve::BezierCurveDescriptorProvider::ResolveTypes(gtf::typesystem::TypeManager const * typeManager)
{
    bool success = true;
    RESOLVE_PROPERTIES(BezierCurve32Properties);
    RESOLVE_PROPERTIES(BezierCurveColorProperties);
    RESOLVE_PROPERTIES(BezierCurveFloatProperties);

    return success;
}

