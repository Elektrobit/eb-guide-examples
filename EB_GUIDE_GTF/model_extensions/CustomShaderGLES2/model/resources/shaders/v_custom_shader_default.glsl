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

uniform highp mat4 u_object2World;

#ifdef GLES3
layout(location = 0) in highp vec2 a_position0;

#else
attribute highp vec2 a_position0;

#endif // GLES3

void main()
{
    vec4 pos  = vec4(a_position0, 0.0, 1.0);
    vec4 spos = u_object2World * pos;

    gl_Position = spos;
}
